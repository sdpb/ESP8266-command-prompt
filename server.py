import socket
import sys


commands_dict = {'': '-1', 'exit': '0', 'led_off': '1',
                 'led_on': '2', 'switch': '3', 'analog': '4'}

commands = list(commands_dict.keys())


# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Bind the socket to the port
server_address = ('127.0.0.1', 1337)
print('Starting up on {} port {}'.format(*server_address))
sock.bind(server_address)

# Listen for incoming connections
sock.listen(1)
print(f'Available commands: {commands}')
print('='*55, '\n')
try:
    while True:
        command_sended = False
        # Wait for a connection
        print('Waiting for a connection')
        connection, client_address = sock.accept()
        print('Connection from {} port {}\n'.format(*client_address))
        try:
            # Receive the data in small chunks and retransmit it
            while True:

                command = input("Command > ")
                command = command if command in commands else ''
                command = commands_dict[command]

                if command:
                    print(f'\nSending data to the client: {command}.')
                    return_data = command
                    return_data = bytes(return_data, 'utf-8')
                    try:
                        connection.sendall(return_data)
                    except BrokenPipeError as bp:
                        print(
                            f'¡Oh, no! {bp}\nConnection lost with the client.')
                        connection.close()
                        break

                    data = connection.recv(64)
                    data = str(data, 'utf-8')

                    print(f'Received data: {data}')
                else:
                    print(
                        f'There is no data to send from {client_address}.')
                    break

        finally:
            # Clean up the connection
            connection.close()

except KeyboardInterrupt:
    print(f'Good bye. :(')
    exit()
