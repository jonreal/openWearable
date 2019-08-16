# Thumbs-Up Project

This branch of openWearable is specific to the thumbs-up project.

## Connecting to beaglebone

From host computer:

```
> ssh root@beaglebone.local
```

The default password is root.

## Apps for Thumbs-Up Experiments

Use the appropiate applications during the protocol as follows:

### thumbsup-gas

This application should be used during the goal attainment scaling (GAS) portion of the protocol. To start the application fro the terminal, run:

```
> thumbsup-gas
```
Once the application has started, some information will be printed to the terminal along with the text-based user interface menu:

```
---------------------------------------------------------------------
Menu: f - start trial
      s - stop trial
      e - exit
-----------------------------------------------------------------------
```

To start a trial press 'f' then 'enter'. The use will be promtped to enter the trial name (e.g., gas-subject1):

```
    		Enter trial name: gas-subject1
		Saving data to /root/openWearable/apps/datalog/subject-1
		Press enter to start trial...
```
After pressing enter to start trial the original menu will appear. To stop a trial press 's' then 'enter', or 'e' then 'enter', which will also exit the application.

### thumbsup-assist

This application should be used during all regular trials/games for the experiment. Start the application from the terminal:

```
> thumbsup-assist -r
```
The '-r' argument specifies to publish a data stream to the ROS via the /openwearable topic.
After initialization the corresponding menu will appear:

```
---------------------------------------------------------------------
 	P0 = 40.00,	dP = 8.00,	thrs = 0.25

Menu: f - start trial
      s - stop trial
      p - change parameter
      e - exit
-----------------------------------------------------------------------
```

The top line in the menu reports the current values of modifiable parameters. Press 'p' then 'enter' to access the parameter change menu:

```
		Choose a parameter to change

		 a - P0 - base pressure
		 s - dP - reflex step size
		 d - thrs - reflex threshold
```

Press the correponding key and press enter to change a parameter, then enter the new value after the prompt.
```
		Enter new value: <new_value_here>
```
The default menu will appear after. Start/stop trials are the same as in the previous example.

## Extracting data from beaglebone
To copy the experiment data from the beaglebone to your local host computer use rsync:

```
> cd [path_to_where_you_want_the_data]
> rsync root@beaglebone.local:~/openWearable/apps/datalog/* ./
```
That above command will copy all data file inside the datalog directory. If you only want a specific data file substitute the wildcard (\*) with the name of the data file.

