// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <vector>
#include "paddle/fluid/lite/api/cxx_api.h"
#include "paddle/fluid/lite/core/mir/use_passes.h"
#include "paddle/fluid/lite/core/op_registry.h"
#include "paddle/fluid/lite/kernels/use_kernels.h"
#include "paddle/fluid/lite/operators/use_ops.h"

// for eval
DEFINE_string(model_dir, "", "");

namespace paddle {
namespace lite {

#ifdef LITE_WITH_ARM
TEST(ResNet50, test) {
  DeviceInfo::Init();
  lite::ExecutorLite predictor;
  std::vector<Place> valid_places({Place{TARGET(kHost), PRECISION(kFloat)},
                                   Place{TARGET(kARM), PRECISION(kFloat)}});

  predictor.Build(FLAGS_model_dir, Place{TARGET(kARM), PRECISION(kFloat)},
                  valid_places);

  auto* input_tensor = predictor.GetInput(0);
  input_tensor->Resize(DDim(std::vector<DDim::value_type>({1, 3, 224, 224})));
  auto* data = input_tensor->mutable_data<float>();
  for (int i = 0; i < input_tensor->dims().production(); i++) {
    data[i] = 1;
  }

  predictor.Run();

  auto* out = predictor.GetOutput(0);
  std::vector<float> results({2.41399175e-04, 4.13724629e-04, 2.64324830e-04,
                              9.68795503e-05, 2.01968738e-04, 8.14945495e-04,
                              7.45922662e-05, 1.76479152e-04, 7.47223166e-05,
                              6.06825110e-04});
  for (int i = 0; i < results.size(); ++i) {
    EXPECT_NEAR(out->data<float>()[i], results[i], 1e-5);
  }
  ASSERT_EQ(out->dims().size(), 2);
  ASSERT_EQ(out->dims()[0], 1);
  ASSERT_EQ(out->dims()[1], 1000);
}
#endif

}  // namespace lite
}  // namespace paddle