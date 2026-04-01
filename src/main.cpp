#include <filesystem>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <unordered_map>
 
#include "graph/Graph.h"
#include "util/Timer.h"
#include "util/Oracle.h"
 
int main() {
 
    auto t = Timer();
 
    // Auto-discover all .mtx and .edges files in mtx/ folder
    std::vector<std::string> file_paths;
    for (const auto& entry : std::filesystem::directory_iterator("mtx")) {
        auto ext = entry.path().extension().string();
        if (ext == ".mtx" || ext == ".edges")
            file_paths.push_back(entry.path().string());
    }
    std::sort(file_paths.begin(), file_paths.end());
 
    auto now = std::chrono::system_clock::now();
    std::time_t lt = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&lt);
 
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << tm.tm_mon + 1
        << std::setw(2) << tm.tm_mday
        << std::setw(2) << tm.tm_hour
        << std::setw(2) << tm.tm_min;
 
    std::string filename = "./out/" + oss.str() + ".csv";
    std::ofstream file(filename);
    file << "graph,heuristic,treewidth,treeheight,td_time_s,h2h_time_s,h2h_size_bytes,avg_degree,N,E\n";
 
    for (const std::string& path : file_paths) {
        std::string name = std::filesystem::path(path).stem().string();
        std::cout << "\n=== " << name << " ===" << std::endl;
 
        Graph size_probe = Graph::from_mtx(path, false, false);
        bool run_h2h = (size_probe.num_vertices <= 500000);
 
        // Base heuristics: min-degree, min-fill, min-fill-reduction
        const std::vector<std::pair<std::string, Graph::Heuristic>> base_heuristics = {
            {"min-degree",         Graph::Heuristic::MIN_DEGREE},
            {"min-fill",           Graph::Heuristic::MIN_FILL},
            {"min-fill-reduction", Graph::Heuristic::MIN_FILL_REDUCTION},
        };
        for (const auto& [label, heuristic] : base_heuristics) {
            Graph graph = Graph::from_mtx(path, false, false);
            std::cout << "  " << label << "..." << std::flush;
 
            t.reset(); t.start();
            auto td_metrics = graph.get_td(heuristic);
            t.stop();
            double td_time = t.elapsed();
 
            auto& td_bags = std::get<1>(td_metrics);
            uint32_t tw = Graph::treewidth(td_bags);
            size_t   th = graph.get_treeheight();
 
            double   h2h_time = 0.0;
            uint32_t h2h_size = 0;
            if (run_h2h) {
                t.reset(); t.start();
                graph.get_h2h();
                t.stop();
                h2h_time = t.elapsed();
                h2h_size = graph.get_h2h_size();
            }
 
            file << name << "," << label << ","
                 << tw << "," << th << "," << td_time << ","
                 << h2h_time << "," << h2h_size << ","
                 << graph.get_avg_degree() << ","
                 << graph.num_vertices << "," << graph.get_num_edges() << "\n";
 
            std::cout << " tw=" << tw << " th=" << th
                      << " td=" << td_time << "s"
                      << (run_h2h ? " h2h=" + std::to_string(h2h_time) + "s" : " [h2h skipped]")
                      << std::endl;
        }
 
        // Hybrid alphas
        const std::unordered_map<std::string, std::vector<float>> alpha_map = {
            {"road-minnesota",         {0.2f, 0.4f, 0.6f, 0.8f}},
            {"road-usroads-48",        {0.4f, 0.6f, 0.8f}},
            {"road-belgium-osm",       {0.6f, 0.8f}},
            {"road-germany-osm",       {0.6f, 0.8f}},
            {"road-great-britain-osm", {0.6f, 0.8f}},
            {"road-italy-osm",         {0.6f, 0.8f}},
            {"road-netherlands-osm",   {0.6f, 0.8f}},
            {"road-roadNET-PA",        {0.6f, 0.8f}},
        };
        const std::vector<float> alphas = alpha_map.count(name)
            ? alpha_map.at(name)
            : std::vector<float>{0.6f, 0.8f};
 
        for (float alpha : alphas) {
            Graph graph = Graph::from_mtx(path, false, false);
 
            std::ostringstream lbl;
            lbl << "hybrid-" << static_cast<int>(alpha * 10);
            std::string heuristic_label = lbl.str();
            std::cout << "  " << heuristic_label << "..." << std::flush;
 
            t.reset(); t.start();
            auto td_metrics = graph.get_td(Graph::Heuristic::HYBRID, alpha);
            t.stop();
            double td_time = t.elapsed();
 
            auto& td_bags = std::get<1>(td_metrics);
            uint32_t tw = Graph::treewidth(td_bags);
            size_t   th = graph.get_treeheight();
 
            double   h2h_time = 0.0;
            uint32_t h2h_size = 0;
            if (run_h2h) {
                t.reset(); t.start();
                graph.get_h2h();
                t.stop();
                h2h_time = t.elapsed();
                h2h_size = graph.get_h2h_size();
            }
 
            file << name << "," << heuristic_label << ","
                 << tw << "," << th << "," << td_time << ","
                 << h2h_time << "," << h2h_size << ","
                 << graph.get_avg_degree() << ","
                 << graph.num_vertices << "," << graph.get_num_edges() << "\n";
 
            std::cout << " tw=" << tw << " th=" << th
                      << " td=" << td_time << "s"
                      << (run_h2h ? " h2h=" + std::to_string(h2h_time) + "s" : " [h2h skipped]")
                      << std::endl;
        }
    }
 
    file.close();
    std::cout << "\nResults written to " << filename << std::endl;
    return 0;
}
