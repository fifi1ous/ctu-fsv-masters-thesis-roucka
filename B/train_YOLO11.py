from ultralytics import YOLO

# --- 1. Train with YOLOv11n for detection---
# load model
model = YOLO("yolov11n.pt") 

# train model
results = model.train(
    data="/yolov11n/data.yaml",
    epochs=100,
    patience=10,                 
    batch=-1,                    
    imgsz=640,              # input image size
    save_period=10,              
    workers=8,                   
    project="/yolov11n/results", 
    name="YOLO_640",             
    seed=42,                     
)

# evaluate model
metrics = model.val()