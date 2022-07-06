import json
import os
import smtplib
from pathlib import Path

from dotenv import load_dotenv
from fastapi import FastAPI, Request
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from pydantic import BaseModel

load_dotenv()

with open("./spot.json", "w") as f:
    json.dump(0, f)

with open("./email.json", "w") as f:
    json.dump(0, f)

list_of_emails = json.loads(os.getenv("EMAIL_LIST"))
server = smtplib.SMTP_SSL(os.getenv("MAIL_SERVER"), 465)
server.ehlo()
server.login(os.getenv("MAIL_ADDRESS"), os.getenv("MAIL_PASSWORD"))
email_text0 = "The parking Spot is not occupied"
email_text1 = "Parking Space is Occupied!!"
sent_subject = "IoT Project: Parking Notification!"
email_text = """\
From: %s
To: %s
Subject: %s
%s
"""

app = FastAPI()

app.mount(
    "/static",
    StaticFiles(directory=Path(__file__).parent.absolute() / "static"),
    name="static",
)

templates = Jinja2Templates(directory="templates")


class ParkingSpot(BaseModel):
    id: int
    occupied: int

@app.get("/")
async def hello_world(request: Request):
    """
    It returns an HTMLResponse object with the content of the index.html file and a status code of 200
    :return: an HTMLResponse object.
    """
    return templates.TemplateResponse("index.html", {"request": request})


@app.post("/update-spot")
async def parking_spot(spot: ParkingSpot):
    """
    If the parking spot is occupied and it wasn't occupied before, send an email to the list of emails

    :param spot: ParkingSpot
    :type spot: ParkingSpot
    :return: a dictionary with a status code of 200.
    """
    with open("./spot.json", "r") as f:
        previous_spot_occupied = json.load(f)
    if previous_spot_occupied != spot.occupied:  # check for toggle
        with open("./spot.json", "w") as f:
            json.dump(spot.occupied, f)

        number_of_emails = 0

        if os.path.isfile("./email.json"):
            with open("./email.json", "r") as f:
                number_of_emails = json.load(f)
        if number_of_emails <= 50:
            server.sendmail(
                os.getenv("MAIL_ADDRESS"),
                list_of_emails,
                email_text
                % (
                    os.getenv("MAIL_ADDRESS"),
                    ", ".join(list_of_emails),
                    sent_subject,
                    email_text1 if spot.occupied else email_text0,
                ),
            )
            number_of_emails += 1
            with open("./email.json", "w") as f:
                json.dump(number_of_emails, f)

    return {"status": 200}


@app.get("/get-spot")
async def get_spot():
    """
    It reads the value of the spot from a file and returns it as a JSON object
    :return: A dictionary with a key of "occupied" and a value of the previous_spot_occupied == 1
    """
    with open("./spot.json", "r") as f:
        previous_spot_occupied = json.load(f)
    return {"occupied": previous_spot_occupied == 1}
