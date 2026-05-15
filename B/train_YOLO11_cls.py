from ultralytics import YOLO

# --- 1. Train with YOLOv11n for detection---
# load model
model = YOLO("/yolov11n_cls/yolo11n-cls.pt") 

# train model
results = model.train(          
    data="/yolov11n_cls/yolo_dataset", 
    epochs=100,
    patience=10,                
    batch=-1,                    
    imgsz=224,                   
    save_period=10,              
    workers=8,                   
    project="/yolov11n/results_cls", 
    name="YOLO_cls",             
    seed=42,                     
)

# evaluate model
metrics = model.val()