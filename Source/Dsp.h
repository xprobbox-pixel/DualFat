#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstdint>
namespace cobalt
{
constexpr double pi = 3.14159265358979323846;
inline double clamp(double v, double a, double b)
{
    return std::clamp(v, a, b);
}
struct Smooth
{
    double v = 0, a = 0;
    void prepare(double sr, double seconds = .02)
    {
        a = std::exp(-1.0 / (sr * seconds));
    }
    double next(double target)
    {
        return v = target + a * (v - target);
    }
};
struct SVF
{
    double s1 = 0, s2 = 0;
    double tick(double x, double g, double k, bool hp)
    {
        double a = 1 / (1 + g * (g + k));
        double v1 = a * (s1 + g * (x - s2));
        double v2 = s2 + g * v1;
        s1 = 2 * v1 - s1;
        s2 = 2 * v2 - s2;
        return hp ? x - k * v1 - v2 : v2;
    }
};
struct Settings
{
    double cutoff = 1000, q = .707, depth = 0, rate = 1, phase = 0;
    int slope = 1, division = 2, timing = 0, wave = 0, polarity = 0;
    bool enabled = true, bypass = false, lfo = false, sync = true, retrigger = true;
};
struct LFO
{
    double phase = 0;
    double held = 0;
    Smooth output;
    static double sampleHold(double cycle)
    {
        uint32_t x = uint32_t(int64_t(std::floor(cycle))) + 0x9e3779b9u;
        x ^= x >> 16;
        x *= 0x7feb352du;
        x ^= x >> 15;
        x *= 0x846ca68bu;
        x ^= x >> 16;
        return double(x) / 2147483647.5 - 1;
    }
    static double wave(int shape, double p, double hold)
    {
        p -= std::floor(p);
        switch (shape)
        {
        case 1:
            return 1 - 4 * std::abs(p - .5);
        case 2:
            return 2 * p - 1;
        case 3:
            return 1 - 2 * p;
        case 4:
            return p < .5 ? 1 : -1;
        case 5:
            return hold;
        default:
            return std::sin(2 * pi * p);
        }
    }
    void prepare(double sr)
    {
        output.prepare(sr, .003);
    }
    double tick(const Settings &s, double sr, double bpm, double ppq, bool locked, bool reset)
    {
        double beats = 4.0 / std::pow(2.0, s.division);
        if (s.timing == 1)
            beats *= 1.5;
        if (s.timing == 2)
            beats *= 2.0 / 3.0;
        double hz = s.sync ? bpm / (60 * beats) : s.rate;

        if (s.sync && locked && s.retrigger)
            phase = ppq / beats;
        else
        {
            if (reset && s.retrigger)
                phase = 0;
            phase += hz / sr;
        }
        held = sampleHold(phase + s.phase);
        double value = wave(s.wave, phase + s.phase, held);
        if (s.polarity == 1)
            value = (value + 1) * .5;
        if (s.polarity == 2)
            value = (value - 1) * .5;
        return output.next(value);
    }
};
struct Filter
{
    std::array<std::array<SVF, 4>, 2> stages{};
    Smooth cutoff, q, active, depth;
    std::array<Smooth, 4> weights;
    LFO lfo;
    double sr = 44100, effective = 1000;
    bool high = false;
    void prepare(double rate, bool hp, const Settings &s)
    {
        sr = rate;
        high = hp;
        stages = {};
        cutoff.prepare(sr);
        cutoff.v = std::log(s.cutoff);
        q.prepare(sr);
        q.v = s.q;
        active.prepare(sr);
        active.v = s.enabled && !s.bypass;
        depth.prepare(sr);
        depth.v = s.lfo ? s.depth : 0;
        for (int i = 0; i < 4; ++i)
        {
            weights[i].prepare(sr);
            weights[i].v = i == s.slope ? 1 : 0;
        }
        lfo = {};
        lfo.prepare(sr);
    }
    void tick(double *frame, const Settings &s, double bpm, double ppq, bool lock, bool reset)
    {
        double mod = lfo.tick(s, sr, bpm, ppq, lock, reset);
        double d = depth.next(s.lfo ? s.depth : 0);
        effective = clamp(std::exp(cutoff.next(std::log(s.cutoff)) + mod * d * std::log(2.0) * 5),
                          20, std::min(20000.0, sr * .45));
        double g = std::tan(pi * effective / sr), resonance = q.next(s.q),
               blend = active.next(s.enabled && !s.bypass);
        std::array<double, 4> w;
        for (int i = 0; i < 4; ++i)
            w[i] = weights[i].next(i == s.slope ? 1 : 0);
        for (int ch = 0; ch < 2; ++ch)
        {
            double x = frame[ch], y = x, sum = 0;
            for (int i = 0; i < 4; ++i)
            {
                y = stages[ch][i].tick(y, g, 1 / (i == 0 ? resonance : .70710678118), high);
                sum += w[i] * y;
            }
            frame[ch] = x + blend * (sum - x);
        }
    }
    static std::complex<double> response(bool hp, double f, double cutoff, double q, double sr,
                                         int stageCount)
    {
        double g = std::tan(pi * clamp(cutoff, 20, sr * .45) / sr),
               w = std::tan(pi * std::min(f, sr * .499) / sr) / g;
        std::complex<double> result = 1;
        for (int i = 0; i < stageCount; ++i)
        {
            double k = 1 / (i == 0 ? q : .70710678118);
            result *= (hp ? -w * w : 1.0) / std::complex<double>(1 - w * w, k * w);
        }
        return result;
    }
};
struct Engine
{
    Filter filters[2];
    Smooth input, output, mix, bypass;
    double sr = 44100;
    void prepare(double rate, const Settings *s)
    {
        sr = rate;
        filters[0].prepare(sr, true, s[0]);
        filters[1].prepare(sr, false, s[1]);
        for (auto *p : {&input, &output, &mix, &bypass})
            p->prepare(sr);
        input.v = output.v = mix.v = 1;
        bypass.v = 0;
    }
    void tick(double *frame, const Settings *s, double bpm, double ppq, bool lock, bool reset,
              double in, double out, double wet, bool off)
    {
        double dry[2] = {frame[0], frame[1]};
        double ig = input.next(in), og = output.next(out), m = mix.next(wet), b = bypass.next(off);
        for (int c = 0; c < 2; ++c)
            frame[c] *= ig;
        filters[0].tick(frame, s[0], bpm, ppq, lock, reset);
        filters[1].tick(frame, s[1], bpm, ppq, lock, reset);
        for (int c = 0; c < 2; ++c)
        {
            double processed = og * (dry[c] * ig * (1 - m) + frame[c] * m);
            frame[c] = processed + b * (dry[c] - processed);
        }
    }
};
} // namespace cobalt
