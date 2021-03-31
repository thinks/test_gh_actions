#include "tph/foo/foo.h"

#include <algorithm>
#include <cstring>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

#include <fftw3.h>

static std::mutex fftw_mtx;
static std::mutex io_mtx;

static int FftwInitThreads() {
#if 1
  static std::once_flag flag;
  static int res = 1; // Non-zero means error.
  const char* func = __func__;
  std::call_once(flag, [func]() {
    std::cerr << "[tid: " << std::this_thread::get_id()
              << "] " << func << "\n";
    res = fftwf_init_threads();
  });
  return res;
#else
  return fftwf_init_threads();
#endif
}

static void FftwPlanWithThreads() {
#if 1
  static std::once_flag flag;
  std::call_once(flag, []() {
    const auto nthreads =
        std::max(static_cast<int>(std::thread::hardware_concurrency()), 1);

    std::cerr << "[tid: " << std::this_thread::get_id()
              << "] fftwf_plan_with_nthreads: " << nthreads << "\n";
    fftwf_plan_with_nthreads(nthreads);
  });
#else
  fftwf_plan_with_nthreads(nthreads);
#endif
}

static fftwf_plan FftwPlanDftC2R(const int width,
                           const int height,
                           const int width_pad,
                           const int height_pad,
                           fftwf_complex *const in,
                           float *const out,
                           const unsigned int flags) {
  // Make sure initialization has been done, no-op if called already.
  //if (!FftwInitThreads()) { 
  //  return nullptr;
  //}
  //FftwPlanWithThreads();

  static constexpr int rank = 2;
  fftwf_iodim dims[2];
  dims[0].n = height;
  dims[0].is = (width / 2) + 1;
  dims[0].os = width + width_pad;

  dims[1].n = width;
  dims[1].is = 1;
  dims[1].os = 1;

  static constexpr int howmany = 1;
  fftwf_iodim howmanydims[1];
  howmanydims[0].n = 1;
  howmanydims[0].is = ((width / 2) + 1) * height;
  howmanydims[0].os = (width + width_pad) * (height + height_pad);

  const char* func = __func__;
  return std::invoke([&]() {
    const std::lock_guard<std::mutex> lock(fftw_mtx);

    auto p = fftwf_plan_guru_dft_c2r(
        rank, dims, howmany, howmanydims, in, out, flags);
    std::cerr << "[tid: " << std::this_thread::get_id() << "] " << func
              << " -> " << p << "\n";
    return p;
  });
}

static void FftwExecuteDftC2R(fftwf_plan plan, fftwf_complex *in, float *out) {
  {
    const std::lock_guard<std::mutex> lock(io_mtx);
      std::cerr << "[tid: " << std::this_thread::get_id() << "] " << __func__
                << " : " << plan << "\n";
  }
  fftwf_execute_dft_c2r(plan, in, out);
}

static void FftwDestroyPlan(fftwf_plan p) {
  const std::lock_guard<std::mutex> lock(fftw_mtx);
    std::cerr << "[tid: " << std::this_thread::get_id() << "] " << __func__
              << " : " << p << "\n";
  fftwf_destroy_plan(p);
}

static void FftwCleanupThreads() {
#if 1
  static std::once_flag flag;
  const char* func = __func__;
  std::call_once(flag, [func]() {
    std::cerr << "[tid: " << std::this_thread::get_id()
              << "] " << func << "\n";
    fftwf_cleanup_threads();
  });
#else
    fftwf_cleanup_threads();
#endif
}

float* FftwAllocReal(const size_t n) {
  // Assume that malloc is thread-safe.
  float* r = fftwf_alloc_real(n);
  {
    const std::lock_guard<std::mutex> lock(io_mtx);
      std::cerr << "[tid: " << std::this_thread::get_id() << "] " << __func__
                << " -> " << r << "\n";
  }

  return r;
}

fftwf_complex* FftwAllocComplex(const size_t n) {
  // Assume that malloc is thread-safe.
  fftwf_complex* c = fftwf_alloc_complex(n);
  {
    const std::lock_guard<std::mutex> lock(io_mtx);
      std::cerr << "[tid: " << std::this_thread::get_id() << "] " << __func__
                << " -> " << c << "\n";
  }
  return c;
}

void FftwFree(void *p) {
  {
    const std::lock_guard<std::mutex> lock(io_mtx);
      std::cerr << "[tid: " << std::this_thread::get_id() << "] " << __func__
                << " -> " << p << "\n";
  }
  fftwf_free(p);
}

// RAII
struct FftwThreadHelper {
  FftwThreadHelper() {
    //FftwInitThreads();
    //int err = FFT::init_threads();
    //EWAV_ASSERT(err != 0, "FFTW thread init error.");
  }

  ~FftwThreadHelper() { 
    //FftwCleanupThreads(); 
  }
};

static FftwThreadHelper h;

namespace tph {

int FooFunc(const char *const str) {
  const auto len = std::strlen(str);
  return static_cast<int>(len);
}

int MyFftw() {

  static constexpr int kWidth = 4096;
  static constexpr int kHeight = 4096;
  static constexpr int kWidthPad = 0;
  static constexpr int kHeightPad = 0;

  // Uninitialized.
  fftwf_complex *in1 = FftwAllocComplex((kWidth + kWidthPad) * (kHeight + kHeightPad));
  fftwf_complex *in2 = FftwAllocComplex((kWidth + kWidthPad) * (kHeight + kHeightPad));
  fftwf_complex *in3 = FftwAllocComplex((kWidth + kWidthPad) * (kHeight + kHeightPad));
  fftwf_complex *in4 = FftwAllocComplex((kWidth + kWidthPad) * (kHeight + kHeightPad));
  float *out1 = FftwAllocReal((kWidth + kWidthPad) * (kHeight + kHeightPad));
  float *out2 = FftwAllocReal((kWidth + kWidthPad) * (kHeight + kHeightPad));
  float *out3 = FftwAllocReal((kWidth + kWidthPad) * (kHeight + kHeightPad));
  float *out4 = FftwAllocReal((kWidth + kWidthPad) * (kHeight + kHeightPad));

  FftwInitThreads();
  FftwPlanWithThreads();

  auto c1 = std::async(std::launch::async, FftwPlanDftC2R, kWidth, kHeight, kWidthPad, kHeightPad, in1, out1, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
  auto c2 = std::async(std::launch::async, FftwPlanDftC2R, kWidth, kHeight, kWidthPad, kHeightPad, in2, out2, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
  auto c3 = std::async(std::launch::async, FftwPlanDftC2R, kWidth, kHeight, kWidthPad, kHeightPad, in3, out3, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
  auto c4 = std::async(std::launch::async, FftwPlanDftC2R, kWidth, kHeight, kWidthPad, kHeightPad, in4, out4, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);

  auto p1 = c1.get();
  auto p2 = c2.get();
  auto p3 = c3.get();
  auto p4 = c4.get();


  // Set "in" data here?
  auto e1 = std::async(std::launch::async, FftwExecuteDftC2R, p1, in1, out1);
  auto e2 = std::async(std::launch::async, FftwExecuteDftC2R, p2, in2, out2);
  auto e3 = std::async(std::launch::async, FftwExecuteDftC2R, p3, in3, out3);
  auto e4 = std::async(std::launch::async, FftwExecuteDftC2R, p4, in4, out4);
  e1.wait();
  e2.wait();
  e3.wait();
  e4.wait();

  auto a1 = std::async(std::launch::async, FftwDestroyPlan, p1);
  auto a2 = std::async(std::launch::async, FftwDestroyPlan, p2);
  auto a3 = std::async(std::launch::async, FftwDestroyPlan, p3);
  auto a4 = std::async(std::launch::async, FftwDestroyPlan, p4);
  a1.wait();
  a2.wait();
  a3.wait();
  a4.wait();

  FftwFree(in1);
  FftwFree(in2);
  FftwFree(in3);
  FftwFree(in4);
  FftwFree(out1);
  FftwFree(out2);
  FftwFree(out3);
  FftwFree(out4);
  FftwCleanupThreads();

  return 0;
}

} // namespace tph
