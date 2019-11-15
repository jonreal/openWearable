#include <QtWidgets>
#include <math.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "rotation_display.h"

#define LOCAL_SERVER_PORT 1500

RotationDisplay::RotationDisplay(QWidget *parent)
    : QWidget(parent)
{
    // painter is update when timer expires
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(33); // 30 frames per sec

    // Set up udp server
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      printf("cannot open socket \n");
      exit(1);
    }
    int broadcast = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof broadcast)
         == -1) {
      perror("setsockopt (SO_BROADCAST)");
      exit(1);
    }
    // bind local server port
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(LOCAL_SERVER_PORT);
    rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
    if(rc<0) {
      printf("cannot bind port number %d \n", LOCAL_SERVER_PORT);
      exit(1);
    }
    printf("waiting for data on port UDP %u\n",LOCAL_SERVER_PORT);
    int flags;
    flags = fcntl(sd,F_GETFL,0);
    assert(flags != -1);
    fcntl(sd, F_SETFL, flags | O_NONBLOCK);

    cursor = 0;
    target = 0;
    setWindowTitle(tr("Tracking Task"));
    resize(1000, 500);

    label = new QLabel(this);
    //label->setFrameStyle(QFrame::Panel | QFrame::Raised);
    sprintf(labelbuffer,"score = %-25.0f\t", 0.0);

    label->setText(labelbuffer);
    label->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    label->setFont(QFont("SansSerif", 40));
    gameon = 0;
    flag = 0;
    score = 0.0;
}


void RotationDisplay::paintEvent(QPaintEvent *)
{

    static const QPoint cursorIndicator[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -95)
    };

    static const QColor black(2, 2, 2);
    //static const QColor targetColor(127, 0, 127);
    //static const QColor cursorColor(0, 127, 127, 191);

    static const QColor targetColor(196, 100, 129, 175);
    static const QColor cursorColor(126, 132, 204, 200);

    // Check for data
    memset(msg,0x0,MAX_PACKET_SIZE);
    cliLen = sizeof(cliAddr);
    n = recvfrom(sd, msg, MAX_PACKET_SIZE, 0,
                  (struct sockaddr *) &cliAddr, &cliLen);
    if (n != -1) {
      while (1) {
        char *token = strtok(msg, "\t");
        tstamp = atof(token);
        token = strtok(NULL, "\t");
        cursor = atof(token);
        token = strtok(NULL,"\t");
        target = atof(token);
        token = strtok(NULL,"\t");
        gameon = (int)atof(token);
        n = recvfrom(sd, msg, MAX_PACKET_SIZE, 0,
                      (struct sockaddr *) &cliAddr, &cliLen);
        if (n == -1) {
          break;
        }
      }

      printf("%f\t%f\t%f\t%i\n", tstamp,cursor,target,gameon);
    }

    if ((flag == 0) && (gameon == 1)) {
     score = 0.0;
      flag = 1;
    }
    if ((flag == 1) && (gameon == 1)) {
      score = score + (180. - abs(target - cursor));
    } else if ((flag == 1) && (gameon == 0)) {
      flag = 0;
    }

    if ((flag == 0) && (gameon == 2)) {
     score = 0.0;
      flag = 1;
      t0 = tstamp;
    }
    if ((flag == 1) && (gameon == 2)) {
      score = 15*5*1000 - (tstamp - t0);
    } else if ((flag == 1) && (gameon == 0)) {
      flag = 0;
    }

    sprintf(labelbuffer,"score = %-25.0f\t\t", score/100.0);
    label->setText(labelbuffer);

    // resize
    int side = qMin(width(), height());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height()/1.25);
    painter.scale(side / 150.0, side / 150.0);

    // target
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(targetColor);
    QPointF p1(0,-95);

    painter.rotate(-target);
    painter.drawEllipse(p1, 10, 10);
    painter.restore();

    painter.save();
    painter.setPen(black);
    for (int i = 0; i < 7; ++i) {
        painter.drawLine(88, 0, 96, 0);
        painter.rotate(-30.0);
    }
    painter.restore();

    // cursor
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(cursorColor);
    QPointF p2(0,-95);

    painter.rotate(-cursor);
    painter.drawEllipse(p2, 2, 2);
    painter.drawConvexPolygon(cursorIndicator, 3);
    painter.restore();

    painter.save();
    painter.setPen(black);

    painter.rotate(6.0);
    for (int j = 1; j < 34; ++j) {
        if (((j + 3) % 5) != 0)
            painter.drawLine(92, 0, 96, 0);
        painter.rotate(-6.0);
    }
    painter.restore();

    // score

}
