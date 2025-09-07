# convert pth to onnx
model_name = "CSOL-Utilities-ResNet18-800x600"
import torch
from torchvision import models
import torch.onnx
import os
dir = os.path.dirname(os.path.abspath(__file__))
# load pth
model = models.resnet18()
num_classes = 5
model.fc = torch.nn.Linear(model.fc.in_features, num_classes)

model.load_state_dict(torch.load(f"{dir}/{model_name}.pth"))
model.eval()
# dummy input
# export to onnx
onnx_path = f"{dir}/{model_name}.onnx"
# 导出 .onnx
dummy_input = (torch.randn(1, 3, 600, 800),)
torch.onnx.export(
    model,
    dummy_input,
    onnx_path,
    export_params=True, # Store trained parameters
    opset_version=11, # ONNX version
    do_constant_folding=True, # Optimize constant folding
    input_names=["screenshot"],
    output_names=["interface_type"],
)
