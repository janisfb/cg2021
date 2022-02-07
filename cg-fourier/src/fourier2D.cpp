//=============================================================================
//
//   Exercise code for the lecture
//   "Computer Graphics"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright (C) Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include "fourier2D.h"

#include <pmp/Timer.h>

//=============================================================================

bool Fourier2D::load_signal(const std::string &filename)
{
    if (!signal_.load(filename))
        return false;

    // initialize all other signal and spectrum members to the same size
    const int M = signal_.width_;
    const int N = signal_.height_;
    spectrum_.resize(M, N);
    filtered_spectrum_.resize(M, N);
    filtered_signal_.resize(M, N);

    return true;
}

//-----------------------------------------------------------------------------

void Fourier2D::FT()
{
    pmp::Timer t;
    t.start();
    std::string colors[3] = {"red", "green", "blue"};
    if (!colored_)
    {
        colors[0] = "grey";
        std::cout << "computing 2D-non-colored-FT..." << std::endl;
    }
    else
    {
        std::cout << "computing 2D-colored-FT..." << std::endl;
    }

    int nc = colored_ ? 3 : 1;
    for (int c = 0; c < nc; c++)
    {
        signal_.set_component(c);
        spectrum_.set_component(c);

        fourier_transform();

        std::cout << "\t" << colors[c] << " channel done." << std::endl;
    }
    std::cout << "done (" << ((int)t.stop().elapsed()) / 1e3 << "s)"
              << std::endl;
}

//-----------------------------------------------------------------------------

void Fourier2D::iFT()
{
    pmp::Timer t;
    t.start();
    std::string colors[3] = {"red", "green", "blue"};
    if (!colored_)
    {
        colors[0] = "grey";
        std::cout << "computing 2D-non-colored-iFT..." << std::endl;
    }
    else
    {
        std::cout << "computing 2D-colored-iFT..." << std::endl;
    }

    int nc = colored_ ? 3 : 1;
    for (int c = 0; c < nc; c++)
    {
        filtered_spectrum_.set_component(c);
        filtered_signal_.set_component(c);

        inverse_fourier_transform();

        std::cout << "\t" << colors[c] << " channel done." << std::endl;
    }
    std::cout << "done (" << ((int)t.stop().elapsed()) / 1e3 << "s)"
              << std::endl;
}

//-----------------------------------------------------------------------------

void Fourier2D::fourier_transform()
{
    Signal2D &f_in = signal_;
    Spectrum2D &F_out = spectrum_;

    const int M = f_in.width_;
    const int N = f_in.height_;

    /**
   * \todo Implement the discrete 2D fourier transformation from `f_in` to
   * `F_out`. The naive version involves 4 stacked for-loops, which is not very
   * efficient, but a good starting point. To speed things up, you can perform a
   * 1D-FT for each row first and filter the resulting colums in a second 1D FT.
   * Feel free to parallelize your code with OpenMP and to use the hermitian
   * symmetry of the FT.
   */
        /**
     * \todo Implement the discrete fourier transformation
     * from `f_in` to `F_out`.
     *
     * Hints:
     * - A complex exponential-function `exp_i(double x)` is
     * implemented in helper.cpp.
     */
    Spectrum2D row_values = Spectrum2D(M, N);
#pragma omp parallel for
    for (int m = 0; m < M; ++m)
    {
        for (int l = 0; l < N; ++l) {
            for (int n = 0; n < N; ++n) {
                row_values(m, l) += f_in(m, n) * exp_i(-2.0 * M_PI * (l - N / 2) * ((double)n / (double)N));
            }
        }
    }

#pragma omp parallel for
    for (int n = 0; n < N; ++n)
    {
        for (int k = 0; k < M; ++k) {
            complex temp = 0;
            for (int m = 0; m < M; ++m) {
                temp += row_values(m, n) * exp_i(-2.0 * M_PI * (k - M / 2) * ((double)m / (double)M));
            }
            F_out(k, n) = (1.0 / sqrt(M * N)) * temp;
        }
    }
     
 /*   for (int l = 0; l < M; ++l)
    {
        complex temp = 0;
        complex temp2 = 0;
        for (int n = 0; n < N; ++n) {
            temp2 = exp_i(-2.0 * M_PI * (k - N / 2) * ((double)n / (double)N));
            for (int m = 0; m < M; ++m) {
                temp += f_in(n, m) * temp2 * exp_i(-2.0 * M_PI * (l - M / 2) * ((double)m / (double)M));
            }
        }
        F_out(k, l) = (1.0 / sqrt(M * N)) * temp;
    }*/
}

//-----------------------------------------------------------------------------

void Fourier2D::inverse_fourier_transform()
{
    Spectrum2D &F_in = filtered_spectrum_;
    Signal2D &f_out = filtered_signal_;

    const int M = F_in.width_;
    const int N = F_in.height_;

    /**
   * \todo Implement the inverse 2D discrete fourier transformation from `F_in`
   * to `f_out`. Speed up your code analogous to the method in
   * `fourier_transform()`.
   */
    Spectrum2D row_values = Spectrum2D(M, N);
#pragma omp parallel for
    for (int m = 0; m < M; ++m)
    {
        for (int n = 0; n < N; ++n)
        {
            row_values(m, n) = (complex)0;
            for (int l = -N/2; l < N/2; ++l) {
                row_values(m, n) += F_in(m, l+N/2) * exp_i(2.0 * M_PI * l * ((double)n / (double)N));
            }
        }
    }

#pragma omp parallel for
    for (int n = 0; n < N; ++n)
    {
        for (int m = 0; m < M; ++m)
        {
            f_out(m, n) = 0.0;
            for (int k = -M/2; k < M/2; ++k) {
                f_out(m, n) += (row_values(k+M/2, n) * exp_i(2.0 * M_PI * k * ((double)m / (double)M))).real();
            }
            f_out(m, n) *= (1.0 / sqrt(M * N));
        }
    }

//#pragma omp parallel for
//    for (int n = 0; n < N; ++n)
//    {
//        for (int m = 0; m < M; ++m)
//        {
//            double temp = 0;
//            for (int k = -M/2; k < M/2; ++k) {
//                for (int l = -N/2; l < N/2; ++l) {
//                    temp += (F_in(k+M/2, l+N/2) * exp_i(2.0 * M_PI * k * ((double)m / (double)M)) * exp_i(2.0 * M_PI * l * ((double)n / (double)N))).real();
//                }
//            }
//            f_out(m, n) = (1.0 / sqrt(M * N)) * temp;
//        }
//    }
}

//-----------------------------------------------------------------------------

void Fourier2D::set_colored(bool colored)
{
    if (colored_ != colored)
    {
        colored_ = colored;

        // set the number of color channels to 1 (not colored) or 3 (colored) for
        // each signal/spectrum
        size_t comp = colored ? 3 : 1;
        signal_.set_num_components(comp);
        spectrum_.set_num_components(comp);
        filtered_spectrum_.set_num_components(comp);
        filtered_signal_.set_num_components(comp);
    }
}

//-----------------------------------------------------------------------------

void Fourier2D::filter()
{
    if (filter_type_ == FilterType::None)
    {
        // simple do-nothing filter, i.e., copy data
        filtered_spectrum_.copy_data(spectrum_);
    }
    else
    {
        // gaussian filters applied to each color channel individually
        int nc = colored_ ? 3 : 1;
        for (int c = 0; c < nc; c++)
        {
            spectrum_.set_component(c);
            filtered_spectrum_.set_component(c);

            switch (filter_type_)
            {
                case FilterType::Gaussian:
                    filter_gaussian();
                    break;
                case FilterType::Inverse_Gaussian:
                    filter_gaussian(true);
                    break;
                case FilterType::None:
                    // nothing to do
                    break;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Fourier2D::filter_gaussian(bool inverse)
{
    Spectrum2D &F_in = spectrum_;
    Spectrum2D &F_out = filtered_spectrum_;

    const int M = F_in.width_;
    const int N = F_in.height_;

    double k_width = kernel_width_;

    /**
   * \todo Filter the data with a gaussian kernel of width `k_width`.
   * Depending on `inverse`, filter (multiply each value) with the gaussian or
   * with an inverse gaussian, i.e. (1 - gaussian).
   *
   * Hints:
   * - You get the desired gaussian from the topmost definition in
   * https://en.wikipedia.org/wiki/Gaussian_function by setting
   * `a = 1`, `b = 0` and `c = k_width`.
   * - The gaussian should be centered (i.e., `x == 0`) at frequency 0.
   */

    for (int m = -M / 2; m < M / 2; ++m) {
        for (int n = -N / 2; n < N / 2; ++n) {
            double ms = pow(m, 2.0) / (2.0 * pow(k_width, 2.0));
            double ns = pow(n, 2.0) / (2.0 * pow(k_width, 2.0));
            double gaussian = exp(-(ms + ns));
            if (inverse) {
                gaussian = 1 - gaussian;
            }
            F_out(m + M / 2, n + N / 2) = F_in(m + M / 2, n + N / 2) * gaussian;
        }
    }
}

//-----------------------------------------------------------------------------

void Fourier2D::hybrid(Signal2D &second_signal)
{
    const int M = signal_.width_;
    const int N = signal_.height_;

    if (second_signal.width_ != M || second_signal.height_ != N)
    {
        std::cerr << "Hybrid dimensions do not match!" << std::endl;
        return;
    }

    FT();
    kernel_width_ = 16;
    auto prev_type = filter_type_;

    filter_type_ = FilterType::Gaussian;
    filter();

    int nc = colored_ ? 3 : 1;
    Spectrum2D temp_spectrum_(M, N);
    temp_spectrum_.set_num_components(nc);

    temp_spectrum_.copy_data(filtered_spectrum_);
    signal_.copy_data(second_signal);

    FT();
    kernel_width_ = 24;

    filter_type_ = FilterType::Inverse_Gaussian;
    filter();

    for (int c = 0; c < nc; c++)
    {
        filtered_spectrum_.set_component(c);
        temp_spectrum_.set_component(c);

#pragma omp parallel for
        for (int j = 0; j < N; j++)
        {
            for (int i = 0; i < M; i++)
            {
                filtered_spectrum_(i, j) += temp_spectrum_(i, j);
            }
        }
    }

    iFT();
    filter_type_ = prev_type;
}

//=============================================================================
