
import torch
_orig_torch_load = torch.load
def _patched_load(*a, **kw):
    kw.setdefault("weights_only", False)
    return _orig_torch_load(*a, **kw)
torch.load = _patched_load

from doclayout_yolo.engine.trainer import BaseTrainer
BaseTrainer.final_eval = lambda self: None
import os
os.environ["WANDB_DISABLED"] = "true"
os.environ["WANDB_MODE"] = "disabled"
from doclayout_yolo import YOLOv10

# --- 1. Train DocLayout-YOLO for detection ---
# load model
model = YOLOv10("doclayout_yolo_docsynth300k_imgsz1600.pt")

# train model
results = model.train(
    data="/doclayout/data.yaml",
    epochs=100,
    patience=20,
    batch=8,                    
    imgsz=1024,
    save_period=10,
    workers=8,
    project="/doclayout/results",
    name="DocLayout_1024",
    amp=False,
    seed=42,
)

# evaluate model
metrics = model.val()
