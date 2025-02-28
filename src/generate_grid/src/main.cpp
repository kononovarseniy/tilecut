#include <cstddef>
#include <iostream>
#include <source_location>
#include <span>
#include <utility>

#include <boost/program_options.hpp>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <mpfr.h>

#include <ka/common/cast.hpp>
#include <ka/common/fixed.hpp>
#include <ka/common/log.hpp>

#include "MPFRDouble.hpp"

namespace ka
{

inline namespace
{

struct Arguments final
{
    /// @brief Output world size.
    f64 world_cells;
    /// @brief Input world size.
    f64 world_size;
    /// @brief Prevents underflows. Does not affect calculated constants.
    /// TODO: Check the value to ensure there are no underflows.
    f64 min_world_coordinate;
    /// @brief Limits absolute error.
    f64 max_world_coordinate;
};

struct Constants final
{
    /// Minimal allowed step of the regular grid.
    /// @brief to_hex(RoundDown(world_size / world_cells))
    std::string min_grid_step;
    /// @brief to_hex(RoundUp(21 * max_world_coordinate * unit_roundoff / min_grid_step))
    std::string min_reliable_fractional_part;
    /// @brief to_hex(RoundDown(1.0 - min_reliable_fractional_part))
    std::string max_reliable_fractional_part;
    /// @brief to_hex(min_world_coordinate)
    std::string min_world_coordinate;
    /// @brief to_hex(max_world_coordinate)
    std::string max_world_coordinate;
};

[[nodiscard]] std::string to_hex(const mpfr_t value)
{
    std::array<char, 32> data;
    mpfr_sprintf(data.data(), "%Ra", value);
    return data.data();
}

[[nodiscard]] std::string to_hex(const long double value)
{
    std::array<char, 32> data;
    mpfr_sprintf(data.data(), "%La", value);
    return data.data();
}

[[nodiscard]] Constants compute_constants(const Arguments & args)
{
    constexpr f64 unit_roundoff = 0x1p-53;

    MPFRDouble min_grid_step;
    mpfr_set_d(min_grid_step, args.world_size, MPFR_RNDD);
    mpfr_div_d(min_grid_step, min_grid_step, args.world_cells, MPFR_RNDD);
    const auto min_step = to_hex(min_grid_step);

    MPFRDouble reliable_fractional_part;
    mpfr_set_d(reliable_fractional_part, args.max_world_coordinate, MPFR_RNDU);
    mpfr_mul_ui(reliable_fractional_part, reliable_fractional_part, 21, MPFR_RNDU);
    mpfr_mul_d(reliable_fractional_part, reliable_fractional_part, unit_roundoff, MPFR_RNDU);
    mpfr_div(reliable_fractional_part, reliable_fractional_part, min_grid_step, MPFR_RNDU);
    const auto min_reliable = to_hex(reliable_fractional_part);
    mpfr_ui_sub(reliable_fractional_part, 1, reliable_fractional_part, MPFR_RNDD);
    const auto max_reliable = to_hex(reliable_fractional_part);

    return {
        .min_grid_step = min_step,
        .min_reliable_fractional_part = min_reliable,
        .max_reliable_fractional_part = max_reliable,
        .min_world_coordinate = to_hex(args.min_world_coordinate),
        .max_world_coordinate = to_hex(args.max_world_coordinate),
    };
}

void generate_constants_cpp(const std::span<char *> args, const std::string_view name, const Constants & constants)
{
    const auto write = [&]<typename... Args>(fmt::format_string<Args...> fmt = "", Args &&... args)
    {
        std::cout << fmt::format(fmt, std::forward<Args>(args)...) << std::endl;
    };

    write("// These values are generated by the following command:");
    write("// ka_generate_grid {}", fmt::join(args, " "));
    write("constexpr GridParameters {} = {{", name);
    write("    .cell_size = {},", constants.min_grid_step);
    write("    .desired_cell_size = {},", constants.min_grid_step);
    write("    .min_input = {},", constants.min_world_coordinate);
    write("    .max_input = {},", constants.max_world_coordinate);
    write("    .column_border_intersecion = {{");
    write("        .min_reliable_fractional_part = {},", constants.min_reliable_fractional_part);
    write("        .max_reliable_fractional_part = {},", constants.max_reliable_fractional_part);
    write("    }},");
    write("}};");
}

} // namespace

} // namespace ka

int main(int argc, char * argv[])
{
    using namespace ka;
    namespace po = boost::program_options;

    constexpr auto option_help = "help";
    constexpr auto option_name = "name";
    constexpr auto option_world_cells = "world-cells";
    constexpr auto option_world_size = "world-size";
    constexpr auto option_min_world_coordinate = "min-world-coordinate";
    constexpr auto option_max_world_coordinate = "max-world-coordinate";

    po::options_description desc("General options");
    desc.add_options()(option_help, "Print this help message.");

    po::options_description desc_format("Output formatting");
    desc_format.add_options()(option_name, po::value<std::string>()->required(), "The name of the generated constant.");

    po::options_description desc_grid("Grid parameters");
    desc_grid.add_options()(
        option_world_cells,
        po::value<std::string>()->required(),
        "The size of the world in grid cells.\n"
        "Together with --world-size, this sets the minimum allowed cell size.\n"
        "The calculated constants can be used for grids with larger cell sizes, but this may result in a decrease in "
        "processing speed.\n"
        "Use the 'inexact:' prefix to allow rounding when converting from decimal representation.");
    desc_grid.add_options()(
        option_world_size,
        po::value<std::string>()->required(),
        "The physical size of the world. Typically this is the length of the equator in the units of choosen "
        "projection, for example 40075016.68 for EPSG:3857.\n"
        "Together with --world-cells, this sets minimum allowed cell size.\n"
        "Use the 'inexact:' prefix to allow rounding when converting from decimal representation.");
    desc_grid.add_options()(
        option_min_world_coordinate,
        po::value<std::string>()->required()->value_name("MIN"),
        "For each X and Y input coordinate assume |coordinate| >= MIN or coordinate == 0\n"
        "Use the 'inexact:' prefix to allow rounding when converting from decimal representation.");
    desc_grid.add_options()(
        option_max_world_coordinate,
        po::value<std::string>()->required()->value_name("MAX"),
        "For each X and Y input coordinate assume |coordinate| <= MAX\n"
        "Use the 'inexact:' prefix to allow rounding when converting from decimal representation.");

    const auto help = [&]()
    {
        const auto example = fmt::format(
            "Example:\n"
            "    ka_generate_gird --{}={} --{}={} --{}={} --{}={} --{}={}\n"
            "\n"
            "    This command will generate constants necessary for cutting geometries\n"
            "    in the EPSG:3857 projection on a 2^32 by 2^32 cell grid with the ability\n"
            "    to wrap geometries not going beyond 2^25.\n",
            option_name,
            "g_epsg_3857_grid",
            option_world_cells,
            "0x1p32",
            option_world_size,
            "inexact:40075016.68",
            option_min_world_coordinate,
            "inexact:0.005",
            option_max_world_coordinate,
            "0x1p25");
        std::cout << desc << example;
    };

    po::variables_map variables;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), variables);

        desc.add(desc_format).add(desc_grid);
        if (variables.count("help"))
        {
            help();
            return EXIT_SUCCESS;
        }

        po::store(po::command_line_parser(argc, argv).options(desc).run(), variables);

        po::notify(variables);
    }
    catch (const boost::program_options::error & e)
    {
        log_error(fmt::format("Failed to parse program options: {}", e.what()));
        help();
        return EXIT_FAILURE;
    }

    const auto parse_float = [](const std::string str)
    {
        MPFRDouble result;

        constexpr std::string_view inexact_prefix = "inexact:";
        const auto exact = !str.starts_with(inexact_prefix);

        const auto begin = exact ? str.c_str() : str.c_str() + inexact_prefix.size();
        char * end;
        const auto rounding = mpfr_strtofr(result, begin, &end, 0, MPFR_RNDN);
        if (end - str.c_str() != exact_cast<ptrdiff_t>(str.size()))
        {
            log_error(fmt::format("Invalid floating-point value: {}", str));
            std::exit(EXIT_FAILURE);
        }
        if (exact && rounding != 0)
        {
            log_error(fmt::format("Floating-point value representation is inexact: '{}'", str));
            log_info(fmt::format("Use '{0}' prefix to allow rounding: '{0}{1}'", inexact_prefix, str));
            std::exit(EXIT_FAILURE);
        }
        return static_cast<f64>(result);
    };

    const auto constants = compute_constants({
        .world_cells = parse_float(variables[option_world_cells].as<std::string>()),
        .world_size = parse_float(variables[option_world_size].as<std::string>()),
        .min_world_coordinate = parse_float(variables[option_min_world_coordinate].as<std::string>()),
        .max_world_coordinate = parse_float(variables[option_max_world_coordinate].as<std::string>()),
    });

    generate_constants_cpp(std::span(argv, argc).subspan(1), variables[option_name].as<std::string>(), constants);

    return EXIT_SUCCESS;
}
