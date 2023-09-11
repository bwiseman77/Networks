import sys, socket, json
from subprocess import run

BUFSIZ = 4096
Commands = {"add", "get", "remove", "update", "getrange"}
Fields = {"date", "time", "duration", "name", "location", "description"}


def main():
    if len(sys.argv) == 1:
        print("No calendar specified")
        help()
        exit(-1)
    elif len(sys.argv) == 2:
        print("No command specified")
        help()
        exit(-1)

    msg = None
    error = None
    calendar = sys.argv[1]
    cmd = sys.argv[2]
    if cmd == "add":
        msg, error = add()
    elif cmd == "get":
        msg, error = get()
    elif cmd == "remove":
        msg, error = remove()
    elif cmd == "update":
        msg, error = update()
    elif cmd == "getrange":
        msg, error = getrange()
    elif cmd == "input":
        input(sys.argv[3])
        exit(0)
    else:
        error = "Client Error: Invalid command"

    if msg != None:
        host, port = getaddrinfo()
        client_socket = connect(host, port)

        r = send_message(client_socket, msg)
        response = json.loads(r[:len(r)-1]) # trim null charachter
		
        print_msg(response)

        close(client_socket)
    elif error != None:
        print(error)

def print_msg(json):
    msg = json["message"]
    if msg == " ":
        return    
    print(msg)
    if len(json["data"].keys()) != 0:
        for event in json["data"].keys():
            if event != "0":
                 print(" ")
            print(f"event {int(event) + 1}")
            for item in json["data"][event].keys():
                print("{:<10} : {:<10}".format(item, json["data"][event][item]))
    print(" ")
    

def isValidDate(date):
    if len(date) == 6 and date.isnumeric():
        if int(date[:2]) <= 12 and int(date[:2]) >= 1:
            if int(date[2:4]) <= 31 and int(date[2:4]) >= 1:
                return True
    return False

def isValidTime(time):
    if len(time) == 4 and time.isnumeric():
        if int(time[:2]) < 24:
            if int(time[2:]) < 60:
                return True
    return False

def isValidDuration(duration):
    return duration.isnumeric()

def connect(host, port):
    client_socket = socket.socket()
    client_socket.connect((host, port))
    return client_socket


def close(client_socket):
    client_socket.close()

def send_message(client_socket, msg):
    message = bytes(msg, "utf-8")
    #print(f'sending: {msg}')

    client_socket.send(message)
    data = client_socket.recv(BUFSIZ)
    return data

def help():
    print("Usage:    ./mycal CalendarName action -> data for action <-")

def add():
    error = None
    calendarEntry = {"date": None, "time": None, "duration": None, "name": None}
    for i in range(3, len(sys.argv), 2):
        if sys.argv[i] not in Fields:
            error = "Client Error: Invalid field name"
        elif len(sys.argv) == i + 1:
            error = "Client Error: No value entered for this field"
        else:
            calendarEntry[sys.argv[i]] = sys.argv[i+1]
    for field in calendarEntry:
        if calendarEntry[field] == None:
            error = f"Required field unspecified: {field}"
    if not isValidDate(calendarEntry["date"]):
        error = "Client Error: Invalid date"
    elif not isValidTime(calendarEntry["time"]):
        error = "Client Error: Invalid time"
    elif not isValidDuration(calendarEntry["duration"]):
        error = "Client Error: Invalid dutration"
    else:
        return f'{sys.argv[1]} {sys.argv[2]} {json.dumps(calendarEntry)}', error

def get():
    if len(sys.argv) != 4:
        return None, "Client Error: Incorrect number of arguments"
    else:
        return f"{sys.argv[1]} get {sys.argv[3]}\n\0", None

def remove():
    if len(sys.argv) != 4:
        return None, "Client Error: Incorrect number of arguments"
    else:
        return f"{sys.argv[1]} remove {sys.argv[3]}\n\0", None

def update():
    if len(sys.argv) != 6:
        return None, "Client Error: Incorrect number of arguments"
    else:
        return f"{sys.argv[1]} update {sys.argv[3]} {sys.argv[4]} {sys.argv[5]}\n\0", None

def getrange():
    if len(sys.argv) != 5:
        return None, "Client Error: Incorrect number of arguments"
    else:
        return f"{sys.argv[1]} getrange {sys.argv[3]} {sys.argv[4]}\n\0", None

def input(file):
    fd = open(file)
    commands = json.load(fd)
    fd.close()

    for cmd in commands:
        args = cmd["arguments"]
        args.insert(0, cmd["command"])
        args.insert(0, sys.argv[1])
        args.insert(0, "src/Client/client.py")
        args.insert(0, "python3")
        run(args)

def getaddrinfo():
    fd = open(".mycal")
    addrinfo = json.load(fd)
    #print(addrinfo["servername"], addrinfo["port"])
    return addrinfo["servername"], int(addrinfo["port"])

if __name__ == "__main__":
    main()
