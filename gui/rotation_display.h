#ifndef ROTATIONDISPLAY_H
#define ROTATIONDISPLAY_H

#include <QWidget>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PACKET_SIZE 1024


class RotationDisplay : public QWidget
{
    Q_OBJECT

public:
  RotationDisplay(QWidget *parent = 0);

private:
  double t, t0;
  int n, sd, rc, b;
  char msg[MAX_PACKET_SIZE];
  struct sockaddr_in cliAddr, servAddr;
  socklen_t cliLen;
  double target, cursor, tstamp;

protected:
  void paintEvent(QPaintEvent *event) override;


};

#endif
