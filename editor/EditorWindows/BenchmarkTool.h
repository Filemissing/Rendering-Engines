#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "EditorWindow.h"

// #include "EditorWindow.h"

namespace editor::editorWindows {

    // -------------------------------------------------------------------------
    // BenchmarkEntry  –  a single named benchmark with N recorded samples
    // -------------------------------------------------------------------------
    struct BenchmarkEntry {
        std::string         name;
        std::vector<double> samples; // milliseconds

        double minVal    = 0.0;
        double maxVal    = 0.0;
        double meanVal   = 0.0;
        double medianVal = 0.0;
        double stdDev    = 0.0;

        void Recompute() {
            if (samples.empty()) return;

            std::vector<double> sorted = samples;
            std::sort(sorted.begin(), sorted.end());

            minVal   = sorted.front();
            maxVal   = sorted.back();
            meanVal  = std::accumulate(sorted.begin(), sorted.end(), 0.0) /
                       static_cast<double>(sorted.size());

            const size_t mid = sorted.size() / 2;
            medianVal = (sorted.size() % 2 == 0)
                            ? (sorted[mid - 1] + sorted[mid]) / 2.0
                            : sorted[mid];

            double variance = 0.0;
            for (double v : sorted)
                variance += (v - meanVal) * (v - meanVal);
            stdDev = std::sqrt(variance / static_cast<double>(sorted.size()));
        }
    };

    // -------------------------------------------------------------------------
    // BenchmarkTool
    // -------------------------------------------------------------------------
    class BenchmarkTool : public EditorWindow {
        using EditorWindow::EditorWindow;

    public:
        // ── Lifecycle ─────────────────────────────────────────────────────────
        void OnEnable() override;
        void OnGUI()    override;

        // ── Configuration ─────────────────────────────────────────────────────
        static int         s_iterationsPerRun;
        static bool        s_autoRecompute;
        static std::string s_csvOutputPath;
        static std::string s_baseOutputPath;

        // ── Benchmark registration ────────────────────────────────────────────
        static void RegisterBenchmark(const std::string& name,
                                      std::function<void(int)> fn);
        static void RemoveBenchmark(const std::string& name);
        static void ClearBenchmarks();

        // ── Manual value injection ────────────────────────────────────────────
        static void PushSample(const std::string& name, double valueMs);

        // ── Tagged sample (encodes a parameter value into the entry name) ─────
        static void PushTaggedSample(const std::string& paramName,
                                     double paramValue, double valueMs);

        // ── Running ───────────────────────────────────────────────────────────
        static void RunBenchmark(const std::string& name);
        static void RunAll();

        // ── CSV export ────────────────────────────────────────────────────────
        static bool ExportCSV(const std::string& path = "");

        // ── Accessors ─────────────────────────────────────────────────────────
        static const std::vector<BenchmarkEntry>& GetEntries() { return s_entries; }

    private:
        struct RegisteredBenchmark {
            std::string              name;
            std::function<void(int)> fn;
        };

        static std::vector<RegisteredBenchmark> s_registered;
        static std::vector<BenchmarkEntry>      s_entries;

        static BenchmarkEntry& GetOrCreateEntry(const std::string& name);

        // ImGui UI state
        static int  s_selectedEntry;
        static char s_pathBuf[512];
    };

} // namespace editor::editorWindows