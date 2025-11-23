#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

struct Point {
    double x;
    double y;
};

static auto dist(const Point& a, const Point& b) -> double {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt((dx * dx) + (dy * dy));
}

auto mst_length(const std::vector<Point>& pts) -> double {
    size_t n = pts.size();
    if (n <= 1)
        return 0.0;

    std::vector<bool> in_mst(n, false);
    std::vector<double> min_edge(n, std::numeric_limits<double>::max());

    min_edge[0] = 0.0;
    double total = 0.0;

    for (size_t i = 0; i < n; ++i) {
        double best = std::numeric_limits<double>::max();
        size_t u = n;

        for (size_t j = 0; j < n; ++j) {
            if (!in_mst[j] && min_edge[j] < best) {
                best = min_edge[j];
                u = j;
            }
        }

        if (u == n)
            break;

        in_mst[u] = true;
        total += min_edge[u];

        for (size_t v = 0; v < n; ++v) {
            if (!in_mst[v]) {
                double d = dist(pts[u], pts[v]);
                min_edge[v] = std::min(d, min_edge[v]);
            }
        }
    }

    return total;
}

auto main(int argc, char* argv[]) -> int32_t {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <platforms_file> <stations_file>\n";
        return 1;
    }

    std::ifstream pf(argv[1]);
    if (!pf) {
        std::cerr << "Failed to open platforms file: " << argv[1] << '\n';
        return 2;
    }

    std::ifstream sf(argv[2]);
    if (!sf) {
        std::cerr << "Failed to open stations file: " << argv[2] << '\n';
        return 3;
    }

    std::vector<Point> platforms;

    double x; // NOLINT(cppcoreguidelines-init-variables)
    double y; // NOLINT(cppcoreguidelines-init-variables)

    while (pf >> x >> y)
        platforms.push_back(Point{x, y});

    std::vector<Point> stations;

    while (sf >> x >> y)
        stations.push_back(Point{x, y});

    if (platforms.empty() || stations.empty()) {
        std::cerr << "No platforms or no stations loaded.\n";
        return 4;
    }

    double min_length = std::numeric_limits<double>::max();

    int best_idx = -1;

    for (size_t i = 0; i < stations.size(); ++i) {
        std::vector<Point> all = platforms;
        all.push_back(stations[i]);
        double length = mst_length(all);

        if (length < min_length) {
            min_length = length;
            best_idx = static_cast<int>(i);
        }
    }

    std::cout << "Best pumping station index: " << best_idx << "\n";
    std::cout << "Total pipeline length: " << std::fixed << std::setprecision(3)
              << min_length << '\n';
}
