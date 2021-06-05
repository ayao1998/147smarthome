#CODE BASED OFF OF https://ron.sh/creating-real-time-charts-with-flask/ AS PROVIDED ON PIAZZA

import json
import random
import time
import datetime

from flask import Flask, Response, render_template, request

application = Flask(__name__)

@application.route("/")
def index():
    return render_template("index.html")


def plot_data():
    written_data = open("data.txt", "r")
    date_dict = {}
    date_dict["Monday"] = [0,0,0]
    date_dict["Tuesday"] = [0,0,0]
    date_dict["Wednesday"] = [0,0,0]
    date_dict["Thursday"] = [0,0,0]
    date_dict["Friday"] = [0,0,0]
    date_dict["Saturday"] = [0,0,0]
    date_dict["Sunday"] = [0,0,0]
    for line in written_data:
        values = line.strip().split(":")
        day = values[0]
        date_dict[day][0] += int(values[1])
        date_dict[day][1] += int(values[2])
        date_dict[day][2] += int(values[3])

    for day in date_dict:
        json_data = json.dumps(
            {
                "labels": [
                    "Monday",
                    "Tuesday",
                    "Wednesday",
                    "Thursday",
                    "Friday",
                    "Saturday",
                    "Sunday",
                ],
                "datasets": [
                    {
                        "label": "A/C",
                        "backgroundColor": "rgba(54, 162, 235, 1)",
                        "data": [round(date_dict[day][0]*5/3600, 2)],
                    },
                    {
                        "label": "Heater",
                        "backgroundColor": "rgba(255, 99, 132, 1)",
                        "data": [round(date_dict[day][1]*5/3600, 2)],
                    },
                    {
                        "label": "Light",
                        "backgroundColor": "rgba(255, 205, 86, 1)",
                        "data": [round(date_dict[day][2]*5/3600, 2)],
                    },
                ],
            }
        )
        yield f"data:{json_data}\n\n"

    written_data.close()

    write_new = open("data.txt", "w")
    write_new.truncate(0)
    for day in date_dict:
        write_new.write(day+":"+str(date_dict[day][0])+":"+str(date_dict[day][1])+":"+str(date_dict[day][2])+"\n")
    write_new.close()

    while True:
        x = 0 #do nothing


def plot_kwh():
    written_data = open("data2.txt", "r")
    date_dict = {}
    date_dict["Monday"] = [0,0,0]
    date_dict["Tuesday"] = [0,0,0]
    date_dict["Wednesday"] = [0,0,0]
    date_dict["Thursday"] = [0,0,0]
    date_dict["Friday"] = [0,0,0]
    date_dict["Saturday"] = [0,0,0]
    date_dict["Sunday"] = [0,0,0]
    for line in written_data:
        values = line.strip().split(":")
        day = values[0]
        date_dict[day][0] += int(values[1])
        date_dict[day][1] += int(values[2])
        date_dict[day][2] += int(values[3])

    for day in date_dict:
        json_data = json.dumps(
            {
                "labels": [
                    "Monday",
                    "Tuesday",
                    "Wednesday",
                    "Thursday",
                    "Friday",
                    "Saturday",
                    "Sunday",
                ],
                "datasets": [
                    {
                        "label": "A/C",
                        "backgroundColor": "rgba(54, 162, 235, 1)",
                        "data": [round((date_dict[day][0]*5/3600)*2000/1000, 2)],
                    },
                    {
                        "label": "Heater",
                        "backgroundColor": "rgba(255, 99, 132, 1)",
                        "data": [round((date_dict[day][1]*5/3600)*300/1000, 2)],
                    },
                    {
                        "label": "Light",
                        "backgroundColor": "rgba(255, 205, 86, 1)",
                        "data": [round((date_dict[day][2]*5/3600)*50/1000, 2)],
                    },
                ],
            }
        )
        yield f"data:{json_data}\n\n"

    written_data.close()

    write_new = open("data2.txt", "w")
    write_new.truncate(0)
    for day in date_dict:
        write_new.write(day+":"+str(date_dict[day][0])+":"+str(date_dict[day][1])+":"+str(date_dict[day][2])+"\n")
    write_new.close()

    while True:
        x = 0 #do nothing



@application.route("/bar-data")
def bar_data():
    return Response(plot_data(), mimetype="text/event-stream")

@application.route("/bar-data2")
def bar_data2():
    return Response(plot_kwh(), mimetype="text/event-stream")

@application.route("/add")
def add_data():
    my_file = open("data.txt", "a")
    my_file.write(datetime.datetime.now().strftime("%A")+":" + request.args.get("ac")+":"+request.args.get("heat")+":"+request.args.get("light")+"\n")
    my_file.close()

    my_file = open("data2.txt", "a")
    my_file.write(datetime.datetime.now().strftime("%A")+":" + request.args.get("ac")+":"+request.args.get("heat")+":"+request.args.get("light")+"\n")
    my_file.close()
    return "ok"


if __name__ == "__main__":
    application.run(host="0.0.0.0", debug=True, threaded=True)
