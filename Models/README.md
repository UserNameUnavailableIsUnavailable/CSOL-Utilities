# 模型

## 环境要求

Linux 或 Windows 平台下，安装 Miniconda 或 Anaconda 后，按如下方式创建虚拟环境：

```powershell
conda create -p ./.venv python=3.10 -y
# PyTorch
pip3 install torch torchvision --index-url https://download.pytorch.org/whl/cu126
# ONNX
conda install onnx onnxruntime -y
conda activate ./.venv
```

使用 Jupyter Notebook 即可进行训练、推理。

## 模型选择

所有训练好的模型文件可在 [CSOL-Utilities-Controller-Models](https://huggingface.co/UserNameIsUnavailable/CSOL-Utilities-Controller-Models) 下载。

### ResNet18（推荐）

考虑到最终的推理环境为 CPU，因此本项目选择轻量级的 ResNet18 作为骨干网络进行训练。
输入图片的宽度和高度分别为 800 和 600。

输出类别依次为：

- 游戏大厅界面（0）
- 游戏房间界面（1）
- 游戏加载界面（2）
- 游戏内界面（3）
- 结算界面（4）

使用 [训练样例](ResNet/train.ipynb) 进行模型训练。

推理时，使用 softmax 归一化后，选取置信度阈值为 0.95，取概率大于置信度阈值的类别作为最终结果。若概率均小于该阈值，则当前界面应当判定为未知，详见 [推理样例](ResNet/infer.ipynb)。

> [!NOTE]
> 后续可能视情况增加更多类别。

### OCR

本项目直接使用了已训练好的轻量 OCR 模型，其权重文件可在 [SWHL/RapidOCR](https://huggingface.co/SWHL/RapidOCR) 中找到。
轻量级 OCR 模型的推理开销相较于其他分类模型而言较大，自 v1.5.3 起作为备选方案使用。