# Sleeping-Barbers-GTK

Multi-threaded C++ GUI program using gtk simulating the work of a barbershop with multiple barbers and customers 
(separate thread for each barber and each customer).

<b>Purpose</b>

In this program, we extend the original so-called sleeping-barber problem to a
multiple (one or two) sleeping barbers problem where many customers visit a barbershop and receive a haircut service
from any one available barber in the shop.

<b>Sleeping-Barbers Problem</b>

A barbershop consists of a waiting room with n chairs and a barber room with m barber chairs. If there are
no customers to be served, all the barbers go to sleep. If a customer enters the barbershop and all chairs
are occupied, then the customer leaves the shop. If all the barbers are busy but chairs are available, then
the customer sits in one of the free chairs. If the barbers are asleep, the customer wakes up one of the
barbers.

###### Run it with : g++ Sleeping_Barbers.cpp -o Sleeping_Barbers `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -lpthread -fpermissive && ./Sleeping_Barber

## Output Screenshots:

![Starting window](https://raw.githubusercontent.com/daggergad7/Sleeping-Barbers-GTK/master/images/1.png)
![Starting window with options](https://raw.githubusercontent.com/daggergad7/Sleeping-Barbers-GTK/master/images/2.png)
![command window](https://raw.githubusercontent.com/daggergad7/Sleeping-Barbers-GTK/master/images/3.png)
