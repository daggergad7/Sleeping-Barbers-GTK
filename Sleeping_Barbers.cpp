// run with
// g++ gui.cpp -o Sleeping_Barber `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -lpthread -fpermissive && ./Sleeping_Barber


#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>


//GUI
static GtkWidget *number1;
static GtkWidget *number2;
static GtkWidget *number3;
static GtkWidget *number4;
static GtkWidget *result;
static GtkWidget *lab1;
static GtkWidget *lab2;
static GtkWidget *lab3;
static GtkWidget *lab4;
static GtkWidget *lab5;
static GtkWidget *vbox;
static GtkWidget *text_view;
static GtkWidget *window;
static GtkWidget *grid;
static GtkWidget *calculate;
GtkTextBuffer *buffer;




//DEFS
void barber();   // barber thread function
void customer(); //customer thread function

sem_t seatMutex;        // Semaphore for the waiting room
sem_t customers;        // Semaphore for customers currently in the waiting room ready to be served
sem_t smfBarber;        // Semaphore for the barber

int runningTime;        // Total time in seconds that the main thread will work
int numberOfFreeSeats;  // Capacity of the waiting room
int customersCount;     // Number of customers in the program that will come to the shop
int customerWait;       // Maximum time a customer would wait to come again to the shop
int getHCn;             // Number of customers who get haircut

pthread_t barberThread;           // Barber's thread
pthread_t customersThreads[20];   // An array of customer threads with max size of 20
pthread_t guithread;   // To update GUI constantly

int *gotHaircut[20];
int gtp=-1;

void addtext(char strr[60]){
  gtk_widget_show_all(window);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));

  gtk_text_buffer_insert_at_cursor(buffer,"\n",1);
  gtk_text_buffer_insert_at_cursor(buffer,strr,strlen(strr));
}

void mainth(){

      char buffer[60];

      getHCn = 0;

      printf("\nProgram is starting\n\n");
      addtext("Program started");


      // Initializing semaphores:
      // zero indicates that this semaphore is not allowed to be shared between processes, 1 is an initial value
      sem_init(&seatMutex,0,1); // the initial value is 1 because we need to allow only one thread to access
      sem_init(&customers,0,0);
      sem_init(&smfBarber,0,0);

      // Creating barber thread:
      pthread_create(&barberThread, NULL, barber, NULL); // barber is the function which the thread will start at
      printf("Barber has been created.\n");
      addtext("Barber thread created");




      // Creating customers threads:
      for (int i = 0; i < customersCount; i++){
  	   pthread_create(&customersThreads[i], NULL, customer, NULL); // customer is the function which the thread will start at
  	   printf<("Customer %u has been created.\n",customersThreads[i]);
       snprintf(buffer, sizeof(buffer),"Customer %u has been created.",customersThreads[i]);
       addtext(buffer);
      }

      // stop the main thread in order to run the other threads
      sleep(runningTime);
      addtext("\n\n");
      printf<("\n\nEnd of the day \n");
      printf<("%d out of %d customers get haircut.",getHCn,customersCount);
      addtext("End of day");

      snprintf(buffer, sizeof(buffer),"%d out of %d customers get haircut.",getHCn,customersCount);
      addtext(buffer);


      pthread_cancel(barberThread);

      for (int i = 0; i < customersCount; i++){
       pthread_cancel(customersThreads[i]);
      }

      for (int i = 0; i <= gtp; i++){
       snprintf(buffer, sizeof(buffer),"Customer %u got haircut",gotHaircut[i]);
       addtext(buffer);
      }


}


// barber thread function:
void barber() {



  char buffer[60];

    int workingTime;    // Will be generated randomly to indicate the time
                    // that the barber will take to cut a customer's hair

    while(1) {
        //===== ENTRY SECTION =====//

        // acquire customers to wait for a customer
	    sem_wait(&customers);
        // acquire seatMutex to access seats count
	    sem_wait(&seatMutex);

	    //===== CRITICAL SECTION =====//

        // increase the number of free seats
	    numberOfFreeSeats += 1;

	    // generate random time between 1-5 seconds for the period of haircut.
	    workingTime = (rand() % 5) + 1;
	    printf("Barber took a new customer, and he will take %d seconds for haircut.\n",workingTime);

	    printf("\tNumber of free seats: %d\n",numberOfFreeSeats);
	    //===== EXIT SECTION =====//

        // signal to customer which mean barber is ready
	    sem_post(&smfBarber);
        // release lock on seat count
	    sem_post(&seatMutex);
   	    sleep(workingTime);
    }
}

// customer thread function:
void customer() {
  char buffer[60];

    int waitingTime;
    int notEnd = 1;
    while(notEnd == 1) {
	    //===== ENTRY SECTION =====//

        // acquire seatMutex to access seats count
	    sem_wait(&seatMutex);


        // when there are no free seats
	    if(numberOfFreeSeats <= 0){

		    //===== EXIT SECTION =====//

	        // generate random time for waiting until next try
	        waitingTime = (rand() % customerWait) + 1;
		    printf("Customer %u left without haircut, and will come back after %d seconds to try again.\n", pthread_self(),waitingTime);
		    sem_post(&seatMutex); // release the semaphore

	        sleep(waitingTime);
	    }


        // when there are free seats
	    else{

		    //===== CRITICAL SECTION =====//

            // decrease the number of free seats
		    numberOfFreeSeats -= 1;
		    printf("Customer %u is waiting.\n",pthread_self());
		    printf("\tNumber of free seats: %d\n",numberOfFreeSeats);

		    //===== EXIT SECTION =====//

            // customer is ready
		    sem_post(&customers);
            // release seatMutex lock on seat count
		    sem_post(&seatMutex);
            // wait for barber
		    sem_wait(&smfBarber);
            // get haircut
		    printf("Customer %u got a haircut\n",pthread_self());
        gotHaircut[++gtp]=pthread_self();
		    // the customer thread will end
		    notEnd = 0;
		    // increse the number of customers who get haircut
		    getHCn += 1;
	    }

     }
}


void startok(GtkWidget *calculate, gpointer data) {

    runningTime = atoi((char *)gtk_entry_get_text(GTK_ENTRY(number1)));
    numberOfFreeSeats = atoi((char *)gtk_entry_get_text(GTK_ENTRY(number2)));
    customersCount = atoi((char *)gtk_entry_get_text(GTK_ENTRY(number3)));
    customerWait = atoi((char *)gtk_entry_get_text(GTK_ENTRY(number4)));
    // printf<("%d",runningTime);

    /* Set the default buffer text. */
    gtk_text_buffer_set_text (buffer, "................................................Result................................................", -1);


    // mainth();

    // addtext("hello")
}


void gui(){
  int argc;
  char **argv;
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  grid = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(window), grid);


  calculate = gtk_button_new_with_label("Start");
  g_signal_connect(calculate, "clicked", G_CALLBACK(startok), NULL);
  sleep(1);
  g_signal_connect_after(calculate, "clicked", G_CALLBACK(mainth), NULL);

  gtk_grid_attach(GTK_GRID(grid), calculate, 1, 6, 1, 1);



  lab1 = gtk_label_new("Running time of program:");
  gtk_grid_attach(GTK_GRID(grid), lab1, 0, 1, 1, 1);


  lab2 = gtk_label_new("Free Seats:");
  gtk_grid_attach(GTK_GRID(grid), lab2, 0, 2, 1, 1);


  lab3 = gtk_label_new("Customers:");
  gtk_grid_attach(GTK_GRID(grid), lab3, 0, 3, 1, 1);


  lab4 = gtk_label_new("Waiting time:");
  gtk_grid_attach(GTK_GRID(grid), lab4, 0, 4, 1, 1);


  number1 = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid), number1, 1, 1, 1, 1);

  number2 = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid), number2, 1, 2, 1, 1);

  number3 = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid), number3, 1, 3, 1, 1);

  number4 = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid), number4, 1, 4, 1, 1);

  /* Create a multiline text widget. */
  text_view = gtk_text_view_new ();
  gtk_grid_attach(GTK_GRID (grid), text_view, 4,0,10,10);

  /* Obtaining the buffer associated with the widget. */
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
  /* Set the default buffer text. */
  gtk_text_buffer_set_text (buffer, "................................................WATING................................................", -1);


}

// gcc 007_gtk.c -o 007_gtk `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
int main(int argc, char **argv) {
    gui();
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
