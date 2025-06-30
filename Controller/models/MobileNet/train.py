'''
Datasets should contain these classes:
- Lobby
- Room
- Loading
- Scenario
- Summary

dataset/
    train/
    test/
'''

import os
import copy
import torch
import torch.nn as nn
import torch.optim as optim
from torchvision import datasets, transforms, models

# 数据集路径
DatasetPath = ""
