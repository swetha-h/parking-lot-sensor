# Parking Lot Sensor
This repository contains 
- [sensor.ino](./sensor.ino) - Code that reads distance from an Ultrasonic sensor to determine if a parking spot is occupied or not.

- [main.py](./main.py) - FastAPI server that 
    - recieves requests from the board regarding the current parking lot status.
    - serves a frontend to check status of parking lot
    - notifies users regarding free spot through email

# Steps to run

- Connect the board and laptop to same network
- Run `ipconfig` on PC to get PC's IP.
- Edit `serverName` in iot_project board code to reflect the current PC's IP
- Create virtual env and install requirements from [requirements.txt](./requirements.txt)
```bash
python -m venv env
.\env\Scripts\activate
pip install -r requirements.txt
```
- Run python server using `uvicorn main:app --host 0.0.0.0`
- Compile and Upload [sensor](./sensor.ino) code from Arduino IDE