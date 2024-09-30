/**
 * 
 * Function：LTM实现
 * Author:joker.mao
 * 
*/

#ifndef _LTM_H_
#define _LTM_H_

#include <assert.h>
#include <math.h>

#define MAX_DIV_NUMS     256
#define MAX_H_V_DIV_NUMS 16
#define ALPHA_A          0.6

#define SAFE_FREE_ARR(mem) if (mem) {delete[] mem;}

template<typename _TI, typename _TO>
class Ltm {
private:
    float *div_img_historm_bins_[MAX_H_V_DIV_NUMS][MAX_H_V_DIV_NUMS];
    _TO   *div_img_map_lut_[MAX_H_V_DIV_NUMS][MAX_H_V_DIV_NUMS];
    _TI   *div_img_data_[MAX_H_V_DIV_NUMS][MAX_H_V_DIV_NUMS];
    int   ave_bins_[MAX_H_V_DIV_NUMS][MAX_H_V_DIV_NUMS];
    int   sd_bins_[MAX_H_V_DIV_NUMS][MAX_H_V_DIV_NUMS];
    int   max_sd_ = 0;
    _TO   *out_;

    int width_  = 0;
    int height_ = 0;
    int bin_nums_ = 0;
    int bin_pixel_nums_ = 0;
    int hor_div_nums_;
    int ver_div_nums_;
    int hor_pixel_nums_per_bin_;
    int ver_pixel_nums_per_bin_;

    int in_max_;
    int out_max_;
    float phase_s = 3.5;
    float phase_d = 0;
public:
    Ltm(const _TI * const src, _TO* out, int w, int h, int h_bins, int v_bins, int input_max, int output_max) {
        out_ = out;

        assert(w % h_bins == 0 && h % v_bins == 0 && src != nullptr && out != nullptr);

        width_  = w;
        height_ = h;
        hor_div_nums_ = h_bins;
        ver_div_nums_ = v_bins;
        hor_pixel_nums_per_bin_ = w / h_bins;
        ver_pixel_nums_per_bin_ = h / v_bins;
        in_max_ = input_max;
        out_max_ = output_max;

        bin_nums_ = hor_div_nums_ * ver_div_nums_;
        bin_pixel_nums_ = hor_pixel_nums_per_bin_ * ver_pixel_nums_per_bin_;
        phase_d = sqrtf((height_ * height_) + (width_ * width_));

        for (int bin_idy = 0; bin_idy < ver_div_nums_; ++bin_idy) {
            for (int bin_idx = 0; bin_idx < hor_div_nums_; ++bin_idx) {
                //malloc data
                div_img_historm_bins_[bin_idx][bin_idy] = new float[input_max];
                div_img_map_lut_[bin_idx][bin_idy]      = new _TO[input_max];
                div_img_data_[bin_idx][bin_idy]         = new _TI[bin_pixel_nums_];
                assert(div_img_historm_bins_[bin_idx][bin_idy]);
                assert(div_img_map_lut_[bin_idx][bin_idy]);
                assert(div_img_data_[bin_idx][bin_idy]);
                memset(div_img_historm_bins_[bin_idx][bin_idy], 0, sizeof(float) * input_max);
                _TI val;
                int sum = 0;
                ave_bins_[bin_idx][bin_idy] = 0;
                sd_bins_[bin_idx][bin_idy] = 0;

                int max = 0;
                int min = in_max_;
                //copy div image
                for (int idy = 0; idy < ver_pixel_nums_per_bin_; ++idy) {
                    for (int idx = 0; idx < hor_pixel_nums_per_bin_; ++idx) {
                        val = div_img_data_[bin_idx][bin_idy][idy * hor_pixel_nums_per_bin_ + idx] = \
                            src[(bin_idy * ver_pixel_nums_per_bin_ + idy) * w + (bin_idx * hor_pixel_nums_per_bin_ + idx)];
                        ++div_img_historm_bins_[bin_idx][bin_idy][val];
                        sum += val;
                        if (min > val) min = val;
                        if (max < val) max = val;
                    }
                }
                sd_bins_[bin_idx][bin_idy] = max - min;
                if (sd_bins_[bin_idx][bin_idy] > max_sd_) max_sd_ = sd_bins_[bin_idx][bin_idy];
                ave_bins_[bin_idx][bin_idy] = sum / bin_pixel_nums_;
                //init end
            }
        }
    };

    ~Ltm() {
        for (int bin_idy = 0; bin_idy < ver_div_nums_; ++bin_idy) {
            for (int bin_idx = 0; bin_idx < hor_div_nums_; ++bin_idx) {
                //malloc data
                SAFE_FREE_ARR(div_img_historm_bins_[bin_idx][bin_idy]); 
                SAFE_FREE_ARR(div_img_map_lut_[bin_idx][bin_idy]);
                SAFE_FREE_ARR(div_img_data_[bin_idx][bin_idy]);
            }
        }
    };

    void HistormBanlance() {
        for (int bin_idy = 0; bin_idy < ver_div_nums_; ++bin_idy) {
            for (int bin_idx = 0; bin_idx < hor_div_nums_; ++bin_idx) {
                float alpha = ALPHA_A * (1 - exp(-(max_sd_ - sd_bins_[bin_idx][bin_idy])));
                div_img_historm_bins_[bin_idx][bin_idy][0] /= bin_pixel_nums_;
                div_img_map_lut_[bin_idx][bin_idy][0] = alpha * (div_img_historm_bins_[bin_idx][bin_idy][0]  * out_max_) \
                                + (1 - alpha) * (0 * out_max_ / in_max_);
                for (int i = 1; i < in_max_; ++i) {
                    div_img_historm_bins_[bin_idx][bin_idy][i] = div_img_historm_bins_[bin_idx][bin_idy][i] / bin_pixel_nums_\
                                                             + div_img_historm_bins_[bin_idx][bin_idy][i - 1];
                    //if (bin_idy == 0 && bin_idx == 0) {
                    //    printf("%d %f\r\n", i, div_img_historm_bins_[bin_idx][bin_idy][i]);
                    //}
                    div_img_map_lut_[bin_idx][bin_idy][i] = alpha * (div_img_historm_bins_[bin_idx][bin_idy][i]  * out_max_) \
                                + (1 - alpha) * (i * out_max_ / in_max_);
                }
            }
        }
    }

    int GetPixelMapVal(int val, int ave, int x, int y) {
        double ws_wd_sum = 0;
        double ws_wd_map_sum = 0;

        for (int bin_idy = 0; bin_idy < ver_div_nums_; ++bin_idy) {
            for (int bin_idx = 0; bin_idx < hor_div_nums_; ++bin_idx) {
                float ws = exp(-(fabs(val - ave_bins_[bin_idx][bin_idy]) / in_max_) / phase_s);
                int hc = bin_idx * hor_pixel_nums_per_bin_ + (hor_pixel_nums_per_bin_ >> 1);
                int vc = bin_idy * ver_pixel_nums_per_bin_ + (ver_pixel_nums_per_bin_ >> 1);
                float wd = exp(-(sqrt((x - hc) * (x - hc) + (y - vc) *(y - vc))) / phase_d);
                ws_wd_sum += (ws * wd);
                ws_wd_map_sum += (div_img_map_lut_[bin_idx][bin_idy][val] * ws * wd);
            }
        }
        int rt = static_cast<int>(ws_wd_map_sum / ws_wd_sum);
        if (rt >= out_max_) {
            rt = out_max_ - 1;
        }
        return rt;
    }

    void RunLtmMap() {
        for (int bin_idy = 0; bin_idy < ver_div_nums_; ++bin_idy) {
            for (int bin_idx = 0; bin_idx < hor_div_nums_; ++bin_idx) {
                for (int idy = 0; idy < ver_pixel_nums_per_bin_; ++idy) {
                    for (int idx = 0; idx < hor_pixel_nums_per_bin_; ++idx) {
                        auto& val = div_img_data_[bin_idx][bin_idy][idy * hor_pixel_nums_per_bin_ + idx];
                        //val = div_img_map_lut_[bin_idx][bin_idy][val];
                        val = GetPixelMapVal(val, ave_bins_[bin_idx][bin_idy], (bin_idx * hor_pixel_nums_per_bin_ + idx), (bin_idy * ver_pixel_nums_per_bin_ + idy));
                    }
                }
            }
        }
    }

    void CombineImg() {
        for (int bin_idy = 0; bin_idy < ver_div_nums_; ++bin_idy) {
            for (int bin_idx = 0; bin_idx < hor_div_nums_; ++bin_idx) {
                //copy div image
                for (int idy = 0; idy < ver_pixel_nums_per_bin_; ++idy) {
                    for (int idx = 0; idx < hor_pixel_nums_per_bin_; ++idx) {
                        out_[(bin_idy * ver_pixel_nums_per_bin_ + idy) * width_ + (bin_idx * hor_pixel_nums_per_bin_ + idx)] \
                            = div_img_data_[bin_idx][bin_idy][idy * hor_pixel_nums_per_bin_ + idx];
                    }
                }
            }
        }
    }

    void Run() {
        HistormBanlance();
        RunLtmMap();
        CombineImg();
    };

    const float* GetDivImgHis(int idx, int idy) const {
        assert((idx < hor_div_nums_) && (idy < ver_div_nums_));
        return div_img_historm_bins_[idx][idy];
    }

    _TI* GetDivImgPtr(int idx, int idy) {
        assert((idx < hor_div_nums_) && (idy < ver_div_nums_));
        return div_img_data_[idx][idy];
    }  
    _TO* GetLutMapPtr(int idx, int idy) {
        assert((idx < hor_div_nums_) && (idy < ver_div_nums_));
        return div_img_map_lut_[idx][idy];
    }
};



#endif