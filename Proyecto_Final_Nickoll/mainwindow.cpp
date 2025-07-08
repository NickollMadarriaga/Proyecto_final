// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlaylist>
#include <QPixmap>
#include <QBitmap>
#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QGraphicsPixmapItem>

class HoverIconEventFilter : public QObject
{
    QPixmap normal, hover;
    QPushButton *boton;
public:
    HoverIconEventFilter(QPixmap normalImg, QPixmap hoverImg, QPushButton *btn)
        : QObject(btn), normal(normalImg), hover(hoverImg), boton(btn) {}

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::Enter) {
            boton->setIcon(QIcon(hover));
        } else if (event->type() == QEvent::Leave) {
            boton->setIcon(QIcon(normal));
        }
        return false;
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1200, 675);
    Inicio();
}

void MainWindow::Inicio()
{
    // VIDEO
    QMediaPlaylist *videoPlaylist = new QMediaPlaylist(this);
    videoPlaylist->addMedia(QUrl("qrc:/Videos/inicio.mp4"));
    videoPlaylist->setPlaybackMode(QMediaPlaylist::Loop);

    videoPlayer = new QMediaPlayer(this);
    videoPlayer->setPlaylist(videoPlaylist);
    videoPlayer->setVolume(0);

    videoWidget = new QVideoWidget(this);
    videoWidget->setGeometry(0, 0, 1200, 675);
    videoWidget->setStyleSheet("background-color: black;");
    videoPlayer->setVideoOutput(videoWidget);
    videoWidget->show();
    videoPlayer->play();

    // AUDIO
    QMediaPlaylist *audioPlaylist = new QMediaPlaylist(this);
    audioPlaylist->addMedia(QUrl("qrc:/Audios/opening.mp3"));
    audioPlaylist->setPlaybackMode(QMediaPlaylist::Loop);

    audioPlayer = new QMediaPlayer(this);
    audioPlayer->setPlaylist(audioPlaylist);
    audioPlayer->setVolume(100);
    audioPlayer->play();

    // Mini funcion que al llamarla crea un boton de una vez con la animacion al pasar el mouse
    auto addBtn = [this](const QString &img, const QString &hover, const QPoint &pos, std::function<void()> cb) {
        QPixmap pixmapNormal(img), pixmapHover(hover);
        QPushButton *boton = new QPushButton(this);
        boton->setIcon(QIcon(pixmapNormal));
        boton->setIconSize(pixmapNormal.size());
        boton->setFlat(true);
        boton->setCursor(Qt::PointingHandCursor);
        boton->setStyleSheet("background-color: transparent;");
        boton->setGeometry(pos.x(), pos.y(), pixmapNormal.width(), pixmapNormal.height());
        boton->setMask(pixmapNormal.createMaskFromColor(Qt::transparent, Qt::MaskInColor));
        boton->installEventFilter(new HoverIconEventFilter(pixmapNormal, pixmapHover, boton));
        connect(boton, &QPushButton::clicked, this, [cb]() { cb(); });
        boton->show();
        menuButtons.append(boton);
    };
    // Creacion de los botones con lo anterior
    addBtn(":/Imagenes/BotonLvl1.png", ":/Imagenes/BotonLvl1M.png", QPoint(150, 500), [this]() {
        // Tambien se envia a lo anterior, esta parte le da la función a los botones
        limpiarMenu();
        Nivel1();
    });

    addBtn(":/Imagenes/BotonLvl2.png", ":/Imagenes/BotonLvl2M.png", QPoint(500, 500), [this]() {
        limpiarMenu();
        Nivel2();
    });

    addBtn(":/Imagenes/BotonLvl3.png", ":/Imagenes/BotonLvl3M.png", QPoint(850, 525), [this]() {
        limpiarMenu();
        Nivel3();
    });

    addBtn(":/Imagenes/BotonSalir.png", ":/Imagenes/BotonSalirM.png", QPoint(850, 100), []() {
        qDebug() << "Saliendo del juego...";
        qApp->quit();
    });
}

void MainWindow::limpiarMenu()
{
    // Limpia el video y el audio del menu al salir de este, para mejor uso de la memoria
    for (QPushButton *btn : menuButtons) {
        btn->hide();
        btn->deleteLater();
    }
    menuButtons.clear();

    if (videoPlayer) {
        videoPlayer->stop();
        videoPlayer->deleteLater();
        videoPlayer = nullptr;
    }
    if (audioPlayer) {
        audioPlayer->stop();
        audioPlayer->deleteLater();
        audioPlayer = nullptr;
    }
    if (videoWidget) {
        videoWidget->hide();
        videoWidget->deleteLater();
        videoWidget = nullptr;
    }
}

void MainWindow::addTransparentButton(const QString &imagenNormal, const QString &imagenHover, const QPoint &posicion, std::function<void()> onClick)
{
    //Se supermone una imagen sobre otra, solo para que al pasar el mouse parezca que los botones tienen efecto

    QPixmap pixmapNormal(imagenNormal);
    QPixmap pixmapHover(imagenHover);

    QPushButton *boton = new QPushButton(this);
    boton->setIcon(QIcon(pixmapNormal));
    boton->setIconSize(pixmapNormal.size());
    boton->setFlat(true);
    boton->setCursor(Qt::PointingHandCursor);
    boton->setStyleSheet("background-color: transparent;");
    boton->setGeometry(posicion.x(), posicion.y(), pixmapNormal.width(), pixmapNormal.height());
    boton->setMask(pixmapNormal.createMaskFromColor(Qt::transparent, Qt::MaskInColor));
    boton->installEventFilter(new HoverIconEventFilter(pixmapNormal, pixmapHover, boton));
    connect(boton, &QPushButton::clicked, this, [onClick]() { onClick(); });
    boton->show();
}

void MainWindow::agregarBotonVolver()
{
    //Boton para volver al menu desde algun nivel
    addTransparentButton(":/Imagenes/Volver.png", ":/Imagenes/VolverM.png", QPoint(30, 30), [this]() {
        vista->setScene(nullptr);
        delete vista;
        vista = nullptr;
        Inicio();
    });
}

void MainWindow::Nivel1()
{
    vista = new QGraphicsView(this);
    vista->setGeometry(0, 0, 1200, 675);
    vista->show();

    QGraphicsScene *scene = new QGraphicsScene(this);
    vista->setScene(scene);

    QGraphicsPixmapItem *fondo = new QGraphicsPixmapItem(QPixmap(":/Imagenes/FondoNivel1.png"));
    scene->addItem(fondo);

    agregarBotonVolver();
    goku = new Personaje(":/Imagenes/GokuSpriteLvl1.png", 63, 69, 4, 1, Personaje::Permanente);
    scene->addItem(goku);
    goku->setPos(400, 300);
    goku->setScale(3);

    goku->iniciarAnimacionEvento();
}

void MainWindow::Nivel2()
{
    vista = new QGraphicsView(this);
    vista->setGeometry(0, 0, 1200, 675);
    vista->show();

    QGraphicsScene *scene = new QGraphicsScene(this);
    vista->setScene(scene);

    QGraphicsPixmapItem *fondo = new QGraphicsPixmapItem(QPixmap(":/Imagenes/FondoNivel2.png"));
    scene->addItem(fondo);

    agregarBotonVolver();

    goku = new Personaje(":/Imagenes/GokuSprite.png", 35, 44, 4, 2, Personaje::Permanente);
    scene->addItem(goku);
    goku->setPos(400, 300);
    goku->setScale(3);

}

void MainWindow::Nivel3()
{
    vista = new QGraphicsView(this);
    vista->setGeometry(0, 0, 1200, 675);
    vista->show();

    QGraphicsScene *scene = new QGraphicsScene(this);
    vista->setScene(scene);

    QGraphicsPixmapItem *fondo = new QGraphicsPixmapItem(QPixmap(":/Imagenes/FondoNivel3.png"));
    scene->addItem(fondo);

    agregarBotonVolver();

    goku = new Personaje(":/Imagenes/GokuSprite.png", 35, 44, 4, 2, Personaje::Permanente);
    scene->addItem(goku);
    goku->setPos(400, 300);
    goku->setScale(3);
}

MainWindow::~MainWindow()
{
    delete ui;
}
