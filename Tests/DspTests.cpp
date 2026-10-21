#include "../Source/Dsp.h"
#include <cstdlib>
#include <iostream>
void check(bool condition, const char *message)
{
    if (!condition)
    {
        std::cerr << message << '\n';
        std::exit(1);
    }
}
int main()
{
    using namespace cobalt;
    for (double sr : {22050., 44100., 48000., 96000., 192000.})
    {
        Settings s[2];
        s[0].cutoff = 120;
        s[1].cutoff = 8400;
        Engine engine;
        engine.prepare(sr, s);
        for (int n = 0; n < 100000; ++n)
        {
            for (int i = 0; i < 2; ++i)
            {
                s[i].cutoff = 20 * std::pow(1000., double(n % 10000) / 9999);
                s[i].q = .5 + 7.5 * double(n % 777) / 776;
                s[i].slope = (n / 101) % 4;
                s[i].wave = (n / 203) % 6;
                s[i].depth = 1;
                s[i].lfo = true;
                s[i].bypass = (n / 111) % 2;
                s[i].enabled = (n / 131) % 2;
            }
            double f[] = {std::sin(n * .1), std::cos(n * .07)};
            engine.tick(f, s, 120, n * 120 / (60 * sr), true, n == 0, 1, 1, .7, false);
            check(std::isfinite(f[0]) && std::isfinite(f[1]), "Non-finite audio");
            check(std::abs(f[0]) < 1000 && std::abs(f[1]) < 1000, "Unstable output");
        }
    }
    for (int stages = 1; stages <= 4; ++stages)
    {
        auto lp = Filter::response(false, 10000, 1000, .707, 48000, stages);
        auto hp = Filter::response(true, 50, 1000, .707, 48000, stages);
        check(std::abs(lp) < std::pow(.02, stages), "LP stopband");
        check(std::abs(hp) < std::pow(.004, stages), "HP stopband");
    }
    // Verify the plotted transfer function against measured steady-state audio.
    for (bool high : {false, true})
        for (int slope = 0; slope < 4; ++slope)
        {
            Settings setting;
            setting.cutoff = 1000;
            setting.q = 1.7;
            setting.slope = slope;
            Filter filter;
            filter.prepare(48000, high, setting);
            double energy = 0;
            for (int n = 0; n < 48000; ++n)
            {
                double frame[] = {std::sin(2 * pi * 1500 * n / 48000), 0};
                filter.tick(frame, setting, 120, 0, false, false);
                if (n >= 24000)
                    energy += frame[0] * frame[0];
                check(frame[1] == 0, "Stereo crosstalk");
            }
            double measured = std::sqrt(energy / 12000);
            double expected = std::abs(Filter::response(high, 1500, 1000, 1.7, 48000, slope + 1));
            check(std::abs(measured - expected) < 1e-8, "Graph/audio response mismatch");
        }
    Settings s[2];
    Engine e;
    e.prepare(48000, s);
    for (int n = 0; n < 30000; ++n)
    {
        double f[] = {.2, -.3};
        e.tick(f, s, 120, 0, false, false, 1, 1, 0, false);
        if (n > 29000)
            check(std::abs(f[0] - .2) < 1e-10 && std::abs(f[1] + .3) < 1e-10, "Dry mix mismatch");
    }
    for (int n = 0; n < 30000; ++n)
    {
        double f[] = {.2, -.3};
        e.tick(f, s, 120, 0, false, false, 2, 3, 1, true);
        if (n > 29000)
            check(std::abs(f[0] - .2) < 1e-10, "Bypass mismatch");
    }
    LFO l;
    l.prepare(48000);
    Settings sync;
    sync.sync = true;
    sync.division = 2;
    sync.retrigger = true;
    l.tick(sync, 48000, 120, .25, true, false);
    check(std::abs(l.phase - .25) < 1e-12, "PPQ sync mismatch");
    std::cout << "DSP tests passed: stability, slopes, graph/audio agreement, stereo isolation, "
                 "dry/wet, bypass, PPQ sync\n";
}
