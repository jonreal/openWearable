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

This application should be used during the goal attainment scaling (GAS) portion of the protocol. To start the application:

```
> thumbsup-gas
```
One the application has started, some information will be printed to the terminal along with the text-based user interface menu:

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


