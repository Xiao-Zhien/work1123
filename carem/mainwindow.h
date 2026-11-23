#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QAction>
#include <QImage>
#include <QSlider>
#include <QPushButton>
#include <QDockWidget>
#include <QStack>
#include <QFrame>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    // File
    void openImage();
    void saveImage();

    // Edit
    void undo();
    void redo();
    void resetImage();

    // View
    void zoomIn();
    void zoomOut();
    void normalSize();

    // Image 濾鏡與幾何變換
    void convertToGrayscale();
    void invertColors();
    void rotateLeft();
    void rotateRight();
    void flipHorizontal();
    void flipVertical();

    // 色彩調整與預覽
    void previewColorAdjustments();
    void applyColorAdjustments();
    void resetSliders();
    void showOriginalPreview();
    void hideOriginalPreview();

private:
    void applyModernStyle();
    void createActions();
    void createMenus();
    void createToolBars();
    void createDockPanels();
    void pushUndoState(const QImage &newImage);
    void updateImageDisplay();
    void updateActionStates();

    // Central Widget 工作區
    QWidget *centralContainer;
    QFrame *emptyStateCard;
    QPushButton *emptyOpenBtn;
    QLabel *stepHintBanner;
    QScrollArea *scrollArea;
    QLabel *imageLabel;

    // 影像資料與歷史紀錄
    QImage originalImage;
    QImage baseImage;
    QImage currentImage;
    QStack<QImage> undoStack;
    QStack<QImage> redoStack;
    double scaleFactor = 1.0;
    bool isHoldingOriginal = false;

    // 選單與工具列 Action
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;

    QAction *undoAct;
    QAction *redoAct;
    QAction *resetAct;

    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;

    QAction *grayscaleAct;
    QAction *invertAct;
    QAction *rotateLeftAct;
    QAction *rotateRightAct;
    QAction *flipHAct;
    QAction *flipVAct;

    // 右側操作面板元件
    QDockWidget *adjustDock;
    QLabel *workflowGuideLabel;
    QSlider *brightnessSlider;
    QSlider *contrastSlider;
    QSlider *saturationSlider;
    QLabel *brightnessValLabel;
    QLabel *contrastValLabel;
    QLabel *saturationValLabel;
    QPushButton *compareOriginalBtn;
    QPushButton *resetSliderBtn;
    QPushButton *applyAdjustBtn;
    QPushButton *quickGrayBtn;
    QPushButton *quickInvertBtn;
    QPushButton *quickRotateRightBtn;
    QPushButton *quickSaveBtn;
    QPushButton *dockExitBtn;
};

#endif // MAINWINDOW_H