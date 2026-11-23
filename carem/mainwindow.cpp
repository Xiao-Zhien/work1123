#include "mainwindow.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyle>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTransform>
#include <QColor>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("StudioVision — 智慧影像編輯器");
    resize(1280, 780);

    applyModernStyle();

    // 1. Central Widget
    centralContainer = new QWidget(this);
    QVBoxLayout *centerLayout = new QVBoxLayout(centralContainer);
    centerLayout->setContentsMargins(16, 12, 16, 16);
    centerLayout->setSpacing(10);

    stepHintBanner = new QLabel("👉 第一步：請先點選左上角「開啟」或下方按鈕載入一張圖片（支援 PNG / JPG / BMP）", centralContainer);
    stepHintBanner->setObjectName("StepHintBanner");
    stepHintBanner->setWordWrap(true);
    centerLayout->addWidget(stepHintBanner);

    // 空白狀態引導卡片
    emptyStateCard = new QFrame(centralContainer);
    emptyStateCard->setObjectName("EmptyStateCard");
    QVBoxLayout *emptyLayout = new QVBoxLayout(emptyStateCard);
    emptyLayout->setAlignment(Qt::AlignCenter);
    emptyLayout->setSpacing(14);

    QLabel *emptyTitle = new QLabel("還沒有載入任何圖片", emptyStateCard);
    emptyTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #111827;");
    emptyTitle->setAlignment(Qt::AlignCenter);

    QLabel *emptyDesc = new QLabel(
        "1. 選擇圖片開始編輯\n"
        "2. 在右側拉動滑桿即時預覽，或使用快速效果\n"
        "3. 按住「按住比對原圖」檢查差異，滿意後再儲存",
        emptyStateCard
        );
    emptyDesc->setStyleSheet("font-size: 13px; color: #4B5563;");
    emptyDesc->setAlignment(Qt::AlignCenter);

    emptyOpenBtn = new QPushButton("立即開啟圖片 (Ctrl+O)", emptyStateCard);
    emptyOpenBtn->setObjectName("PrimaryButton");
    emptyOpenBtn->setMinimumHeight(44);
    emptyOpenBtn->setMinimumWidth(240);
    connect(emptyOpenBtn, &QPushButton::clicked, this, &MainWindow::openImage);

    emptyLayout->addWidget(emptyTitle);
    emptyLayout->addWidget(emptyDesc);
    emptyLayout->addWidget(emptyOpenBtn, 0, Qt::AlignHCenter);

    // 影像捲動區
    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);

    scrollArea = new QScrollArea(centralContainer);
    scrollArea->setObjectName("CanvasArea");
    scrollArea->setWidget(imageLabel);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setVisible(false);

    centerLayout->addWidget(emptyStateCard, 1);
    centerLayout->addWidget(scrollArea, 1);

    setCentralWidget(centralContainer);

    // 2. 建立 Action、Menu、ToolBar 與右側操作面板
    createActions();
    createMenus();
    createToolBars();
    createDockPanels();

    updateActionStates();
    statusBar()->showMessage("就緒 — 等待載入圖片");
}

void MainWindow::applyModernStyle()
{
    setStyleSheet(R"(
        QMainWindow {
            background-color: #FFFFFF;
            color: #1F2937;
        }
        QMenuBar {
            background-color: #FFFFFF;
            color: #1F2937;
            border-bottom: 1px solid #E5E7EB;
            padding: 4px;
        }
        QMenuBar::item {
            color: #1F2937;
            padding: 4px 8px;
        }
        QMenuBar::item:selected {
            background-color: #2563EB;
            color: #FFFFFF;
            border-radius: 4px;
        }
        QMenu {
            background-color: #FFFFFF;
            color: #1F2937;
            border: 1px solid #D1D5DB;
            padding: 4px;
        }
        QMenu::item {
            color: #1F2937;
            padding: 5px 16px;
        }
        QMenu::item:selected {
            background-color: #2563EB;
            color: #FFFFFF;
            border-radius: 4px;
        }
        QToolBar {
            background-color: #F9FAFB;
            color: #1F2937;
            border-bottom: 1px solid #E5E7EB;
            spacing: 6px;
            padding: 6px;
        }
        QToolButton {
            background-color: #FFFFFF;
            color: #1F2937;
            border: 1px solid #D1D5DB;
            border-radius: 6px;
            padding: 6px 10px;
            min-height: 28px;
            font-weight: 500;
        }
        QToolButton:hover:!disabled {
            background-color: #F3F4F6;
            border-color: #9CA3AF;
            color: #111827;
        }
        QToolButton:disabled {
            color: #9CA3AF;
            background-color: #F9FAFB;
            border-color: #E5E7EB;
        }
        QDockWidget {
            color: #111827;
            font-weight: bold;
            background-color: #FFFFFF;
        }
        QDockWidget::title {
            background-color: #F9FAFB;
            color: #111827;
            padding: 8px;
            border-bottom: 1px solid #E5E7EB;
        }
        QGroupBox {
            background-color: #FFFFFF;
            border: 1px solid #E5E7EB;
            border-radius: 8px;
            margin-top: 10px;
            padding: 12px 10px 10px 10px;
            font-weight: bold;
            color: #111827;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 6px;
            color: #111827;
        }
        QLabel {
            color: #1F2937;
        }
        QPushButton {
            background-color: #FFFFFF;
            color: #1F2937;
            border: 1px solid #D1D5DB;
            border-radius: 6px;
            padding: 7px 10px;
            min-height: 32px;
            font-weight: 500;
        }
        QPushButton:hover:!disabled {
            background-color: #F3F4F6;
            border-color: #9CA3AF;
            color: #111827;
        }
        QPushButton:pressed:!disabled {
            background-color: #E5E7EB;
        }
        QPushButton:disabled {
            background-color: #F9FAFB;
            color: #9CA3AF;
            border-color: #E5E7EB;
        }
        QPushButton#PrimaryButton {
            background-color: #EFF6FF;
            border: 2px solid #2563EB;
            color: #1D4ED8;
            font-weight: bold;
        }
        QPushButton#PrimaryButton:hover:!disabled {
            background-color: #DBEAFE;
            color: #1E40AF;
        }
        QPushButton#PrimaryButton:disabled {
            background-color: #F9FAFB;
            border: 1px solid #E5E7EB;
            color: #9CA3AF;
        }
        QPushButton#SuccessButton {
            background-color: #ECFDF5;
            border: 2px solid #059669;
            color: #047857;
            font-weight: bold;
        }
        QPushButton#SuccessButton:hover:!disabled {
            background-color: #D1FAE5;
            color: #065F46;
        }
        QPushButton#SuccessButton:disabled {
            background-color: #F9FAFB;
            border: 1px solid #E5E7EB;
            color: #9CA3AF;
        }
        QPushButton#DangerButton {
            background-color: #FEF2F2;
            border: 2px solid #DC2626;
            color: #B91C1C;
            font-weight: bold;
        }
        QPushButton#DangerButton:hover:!disabled {
            background-color: #FEE2E2;
            color: #991B1B;
        }
        QSlider::groove:horizontal {
            height: 6px;
            background: #E5E7EB;
            border-radius: 3px;
        }
        QSlider::sub-page:horizontal {
            background: #2563EB;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #FFFFFF;
            border: 2px solid #2563EB;
            width: 14px;
            height: 14px;
            margin: -5px 0;
            border-radius: 8px;
        }
        QFrame#EmptyStateCard {
            background-color: #FFFFFF;
            border: 2px dashed #CBD5E1;
            border-radius: 12px;
            padding: 36px;
        }
        QLabel#StepHintBanner {
            background-color: #EFF6FF;
            border: 1px solid #BFDBFE;
            border-left: 4px solid #2563EB;
            border-radius: 6px;
            padding: 8px 12px;
            color: #1E3A8A;
            font-size: 13px;
        }
        QScrollArea#CanvasArea {
            background-color: #F8FAFC;
            border: 1px solid #E2E8F0;
            border-radius: 8px;
        }
        QStatusBar {
            background-color: #F9FAFB;
            color: #374151;
            border-top: 1px solid #E5E7EB;
        }
    )");
}

void MainWindow::createActions()
{
    openAct = new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton), "開啟", this);
    openAct->setShortcut(QKeySequence::Open);
    openAct->setShortcutContext(Qt::ApplicationShortcut);
    openAct->setToolTip("開啟圖片 (Ctrl+O)");
    connect(openAct, &QAction::triggered, this, &MainWindow::openImage);

    saveAct = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "存檔", this);
    saveAct->setShortcut(QKeySequence::Save);
    saveAct->setShortcutContext(Qt::ApplicationShortcut);
    saveAct->setToolTip("儲存結果 (Ctrl+S)");
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveImage);

    exitAct = new QAction(style()->standardIcon(QStyle::SP_DialogCloseButton), "離開", this);
    exitAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    exitAct->setShortcutContext(Qt::ApplicationShortcut);
    exitAct->setToolTip("離開程式 (Ctrl+Q)");
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    undoAct = new QAction(style()->standardIcon(QStyle::SP_ArrowBack), "復原", this);
    undoAct->setShortcut(QKeySequence::Undo);
    undoAct->setShortcutContext(Qt::ApplicationShortcut);
    undoAct->setToolTip("復原上一步 (Ctrl+Z)");
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

    redoAct = new QAction(style()->standardIcon(QStyle::SP_ArrowForward), "重做", this);
    redoAct->setShortcut(QKeySequence::Redo);
    redoAct->setShortcutContext(Qt::ApplicationShortcut);
    redoAct->setToolTip("重做下一步 (Ctrl+Y)");
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

    resetAct = new QAction(style()->standardIcon(QStyle::SP_BrowserReload), "還原", this);
    resetAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    resetAct->setShortcutContext(Qt::ApplicationShortcut);
    resetAct->setToolTip("還原為原始圖片");
    connect(resetAct, &QAction::triggered, this, &MainWindow::resetImage);

    zoomInAct = new QAction(style()->standardIcon(QStyle::SP_ArrowUp), "放大", this);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setShortcutContext(Qt::ApplicationShortcut);
    zoomInAct->setToolTip("放大檢視畫面");
    connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);

    zoomOutAct = new QAction(style()->standardIcon(QStyle::SP_ArrowDown), "縮小", this);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setShortcutContext(Qt::ApplicationShortcut);
    zoomOutAct->setToolTip("縮小檢視畫面");
    connect(zoomOutAct, &QAction::triggered, this, &MainWindow::zoomOut);

    normalSizeAct = new QAction(style()->standardIcon(QStyle::SP_FileDialogDetailedView), "100%", this);
    normalSizeAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    normalSizeAct->setShortcutContext(Qt::ApplicationShortcut);
    normalSizeAct->setToolTip("恢復 100% 原始比例");
    connect(normalSizeAct, &QAction::triggered, this, &MainWindow::normalSize);

    grayscaleAct = new QAction("灰階轉換 (&Grayscale)", this);
    grayscaleAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    grayscaleAct->setShortcutContext(Qt::ApplicationShortcut);
    connect(grayscaleAct, &QAction::triggered, this, &MainWindow::convertToGrayscale);

    invertAct = new QAction("色彩反轉 / 負片 (&Invert)", this);
    invertAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    invertAct->setShortcutContext(Qt::ApplicationShortcut);
    connect(invertAct, &QAction::triggered, this, &MainWindow::invertColors);

    rotateLeftAct = new QAction("向左旋轉 90°", this);
    connect(rotateLeftAct, &QAction::triggered, this, &MainWindow::rotateLeft);

    rotateRightAct = new QAction("向右旋轉 90°", this);
    connect(rotateRightAct, &QAction::triggered, this, &MainWindow::rotateRight);

    flipHAct = new QAction("水平翻轉", this);
    connect(flipHAct, &QAction::triggered, this, &MainWindow::flipHorizontal);

    flipVAct = new QAction("垂直翻轉", this);
    connect(flipVAct, &QAction::triggered, this, &MainWindow::flipVertical);
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    QMenu *editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(resetAct);

    QMenu *viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);

    QMenu *imageMenu = menuBar()->addMenu("&Image");
    imageMenu->addAction(grayscaleAct);
    imageMenu->addAction(invertAct);
    imageMenu->addSeparator();
    imageMenu->addAction(rotateLeftAct);
    imageMenu->addAction(rotateRightAct);
    imageMenu->addSeparator();
    imageMenu->addAction(flipHAct);
    imageMenu->addAction(flipVAct);
}

void MainWindow::createToolBars()
{
    QToolBar *mainToolBar = addToolBar("Main ToolBar");
    mainToolBar->setMovable(false);
    mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    mainToolBar->addAction(openAct);
    mainToolBar->addAction(saveAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(undoAct);
    mainToolBar->addAction(redoAct);
    mainToolBar->addAction(resetAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(zoomInAct);
    mainToolBar->addAction(zoomOutAct);
    mainToolBar->addAction(normalSizeAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(exitAct);
}

void MainWindow::createDockPanels()
{
    adjustDock = new QDockWidget("操作與調整面板", this);
    adjustDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    adjustDock->setMinimumWidth(335);

    QWidget *panelWidget = new QWidget(adjustDock);
    panelWidget->setStyleSheet("background-color: #FFFFFF;");
    QVBoxLayout *mainLayout = new QVBoxLayout(panelWidget);
    mainLayout->setSpacing(10);

    // 步驟引導卡
    QGroupBox *guideBox = new QGroupBox("操作步驟指引", panelWidget);
    QVBoxLayout *guideLayout = new QVBoxLayout(guideBox);
    workflowGuideLabel = new QLabel(
        "① 先點「開啟圖片」載入檔案\n"
        "② 調整下方滑桿或點選快速效果\n"
        "③ 按「套用變更」保留，滿意後再「儲存結果」",
        guideBox
        );
    workflowGuideLabel->setStyleSheet("font-size: 12px; color: #374151;");
    workflowGuideLabel->setWordWrap(true);
    guideLayout->addWidget(workflowGuideLabel);
    mainLayout->addWidget(guideBox);

    // 色彩調整滑桿區
    QGroupBox *adjGroup = new QGroupBox("色彩微調（拖曳即時預覽）", panelWidget);
    QVBoxLayout *adjLayout = new QVBoxLayout(adjGroup);

    auto addSlider = [&](const QString &name, QSlider *&slider, QLabel *&valLabel) {
        QHBoxLayout *row = new QHBoxLayout();
        QLabel *titleLbl = new QLabel(name, adjGroup);
        valLabel = new QLabel("0", adjGroup);
        valLabel->setStyleSheet("color: #2563EB; font-weight: bold;");
        valLabel->setAlignment(Qt::AlignRight);

        row->addWidget(titleLbl);
        row->addWidget(valLabel);

        slider = new QSlider(Qt::Horizontal, adjGroup);
        slider->setRange(-100, 100);
        slider->setValue(0);

        adjLayout->addLayout(row);
        adjLayout->addWidget(slider);

        connect(slider, &QSlider::valueChanged, this, [this, valLabel](int value) {
            valLabel->setText(value > 0 ? "+" + QString::number(value) : QString::number(value));
            previewColorAdjustments();
        });
    };

    addSlider("亮度 (Brightness)", brightnessSlider, brightnessValLabel);
    addSlider("對比度 (Contrast)", contrastSlider, contrastValLabel);
    addSlider("飽和度 (Saturation)", saturationSlider, saturationValLabel);

    compareOriginalBtn = new QPushButton("按住比對原圖", adjGroup);
    compareOriginalBtn->setToolTip("按住滑鼠左鍵可暫時看原始圖片，放開恢復目前效果");
    connect(compareOriginalBtn, &QPushButton::pressed, this, &MainWindow::showOriginalPreview);
    connect(compareOriginalBtn, &QPushButton::released, this, &MainWindow::hideOriginalPreview);
    adjLayout->addWidget(compareOriginalBtn);

    QHBoxLayout *sliderBtnRow = new QHBoxLayout();
    resetSliderBtn = new QPushButton("重設滑桿", adjGroup);
    applyAdjustBtn = new QPushButton("套用變更", adjGroup);
    applyAdjustBtn->setObjectName("PrimaryButton");

    connect(resetSliderBtn, &QPushButton::clicked, this, [this]() {
        resetSliders();
        currentImage = baseImage;
        updateImageDisplay();
    });
    connect(applyAdjustBtn, &QPushButton::clicked, this, &MainWindow::applyColorAdjustments);

    sliderBtnRow->addWidget(resetSliderBtn);
    sliderBtnRow->addWidget(applyAdjustBtn);
    adjLayout->addLayout(sliderBtnRow);

    mainLayout->addWidget(adjGroup);

    // 常用一鍵操作區
    QGroupBox *quickGroup = new QGroupBox("常用一鍵操作", panelWidget);
    QGridLayout *quickGrid = new QGridLayout(quickGroup);

    quickGrayBtn = new QPushButton("黑白灰階", quickGroup);
    quickInvertBtn = new QPushButton("負片反轉", quickGroup);
    quickRotateRightBtn = new QPushButton("向右轉 90°", quickGroup);
    quickSaveBtn = new QPushButton("儲存結果", quickGroup);
    quickSaveBtn->setObjectName("SuccessButton");

    connect(quickGrayBtn, &QPushButton::clicked, this, &MainWindow::convertToGrayscale);
    connect(quickInvertBtn, &QPushButton::clicked, this, &MainWindow::invertColors);
    connect(quickRotateRightBtn, &QPushButton::clicked, this, &MainWindow::rotateRight);
    connect(quickSaveBtn, &QPushButton::clicked, this, &MainWindow::saveImage);

    quickGrid->addWidget(quickGrayBtn, 0, 0);
    quickGrid->addWidget(quickInvertBtn, 0, 1);
    quickGrid->addWidget(quickRotateRightBtn, 1, 0);
    quickGrid->addWidget(quickSaveBtn, 1, 1);

    mainLayout->addWidget(quickGroup);
    mainLayout->addStretch();

    // 右側底部顯眼離開按鍵
    dockExitBtn = new QPushButton("離開程式 (Ctrl+Q)", panelWidget);
    dockExitBtn->setObjectName("DangerButton");
    dockExitBtn->setMinimumHeight(38);
    connect(dockExitBtn, &QPushButton::clicked, this, &QWidget::close);
    mainLayout->addWidget(dockExitBtn);

    adjustDock->setWidget(panelWidget);
    addDockWidget(Qt::RightDockWidgetArea, adjustDock);
}

void MainWindow::openImage()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "開啟圖片", "", "Images (*.png *.jpg *.jpeg *.bmp *.webp)"
        );
    if (filePath.isEmpty())
        return;

    QImage loaded(filePath);
    if (loaded.isNull()) {
        QMessageBox::warning(this, "無法開啟圖片", "請確認檔案是否損毀或格式受支援。");
        return;
    }

    originalImage = loaded.convertToFormat(QImage::Format_ARGB32);
    baseImage = originalImage;
    currentImage = originalImage;
    undoStack.clear();
    redoStack.clear();
    scaleFactor = 1.0;

    emptyStateCard->setVisible(false);
    scrollArea->setVisible(true);
    stepHintBanner->setText("👉 第二步：可拖曳右側滑桿即時預覽，或點選「黑白灰階／負片反轉／向右轉 90°」；滿意後即可按「儲存結果」。");

    resetSliders();
    updateImageDisplay();
    updateActionStates();
}

void MainWindow::saveImage()
{
    if (currentImage.isNull())
        return;

    QString filePath = QFileDialog::getSaveFileName(
        this, "儲存圖片", "result.png",
        "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp)"
        );
    if (filePath.isEmpty())
        return;

    if (!currentImage.save(filePath)) {
        QMessageBox::warning(this, "儲存失敗", "無法寫入檔案，請檢查資料夾權限或磁碟空間。");
        return;
    }

    stepHintBanner->setText("✅ 已儲存完成！你可繼續調整圖片，或開啟下一張圖片。");
    statusBar()->showMessage("已儲存至：" + filePath, 5000);
}

void MainWindow::pushUndoState(const QImage &newImage)
{
    if (!baseImage.isNull()) {
        undoStack.push(baseImage);
        redoStack.clear();
    }
    baseImage = newImage;
    currentImage = newImage;
    resetSliders();
    updateImageDisplay();
    updateActionStates();
}

void MainWindow::undo()
{
    if (undoStack.isEmpty())
        return;

    redoStack.push(baseImage);
    baseImage = undoStack.pop();
    currentImage = baseImage;
    resetSliders();
    updateImageDisplay();
    updateActionStates();
}

void MainWindow::redo()
{
    if (redoStack.isEmpty())
        return;

    undoStack.push(baseImage);
    baseImage = redoStack.pop();
    currentImage = baseImage;
    resetSliders();
    updateImageDisplay();
    updateActionStates();
}

void MainWindow::resetImage()
{
    if (originalImage.isNull())
        return;

    pushUndoState(originalImage);
    scaleFactor = 1.0;
    updateImageDisplay();
}

void MainWindow::zoomIn()
{
    scaleFactor *= 1.25;
    updateImageDisplay();
}

void MainWindow::zoomOut()
{
    scaleFactor *= 0.8;
    if (scaleFactor < 0.1) scaleFactor = 0.1;
    updateImageDisplay();
}

void MainWindow::normalSize()
{
    scaleFactor = 1.0;
    updateImageDisplay();
}

void MainWindow::convertToGrayscale()
{
    if (baseImage.isNull()) return;
    QImage gray = baseImage.convertToFormat(QImage::Format_Grayscale8).convertToFormat(QImage::Format_ARGB32);
    pushUndoState(gray);
}

void MainWindow::invertColors()
{
    if (baseImage.isNull()) return;
    QImage inv = baseImage;
    inv.invertPixels();
    pushUndoState(inv);
}

void MainWindow::rotateLeft()
{
    if (baseImage.isNull()) return;
    QTransform tf;
    tf.rotate(-90);
    pushUndoState(baseImage.transformed(tf));
}

void MainWindow::rotateRight()
{
    if (baseImage.isNull()) return;
    QTransform tf;
    tf.rotate(90);
    pushUndoState(baseImage.transformed(tf));
}

void MainWindow::flipHorizontal()
{
    if (baseImage.isNull()) return;
    pushUndoState(baseImage.mirrored(true, false));
}

void MainWindow::flipVertical()
{
    if (baseImage.isNull()) return;
    pushUndoState(baseImage.mirrored(false, true));
}

void MainWindow::previewColorAdjustments()
{
    if (baseImage.isNull())
        return;

    int brightness = brightnessSlider->value();
    int contrast = contrastSlider->value();
    int saturation = saturationSlider->value();

    double contrastFactor = (259.0 * (contrast + 255.0)) / (255.0 * (259.0 - contrast));
    double satFactor = 1.0 + (saturation / 100.0);

    QImage result = baseImage.copy();
    for (int y = 0; y < result.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            QColor c = QColor::fromRgb(line[x]);
            int alpha = qAlpha(line[x]);

            int h, s, v;
            c.getHsv(&h, &s, &v);
            s = std::clamp(static_cast<int>(s * satFactor), 0, 255);
            c.setHsv(h, s, v);

            int r = std::clamp(static_cast<int>(contrastFactor * (c.red() - 128) + 128 + brightness), 0, 255);
            int g = std::clamp(static_cast<int>(contrastFactor * (c.green() - 128) + 128 + brightness), 0, 255);
            int b = std::clamp(static_cast<int>(contrastFactor * (c.blue() - 128) + 128 + brightness), 0, 255);

            line[x] = qRgba(r, g, b, alpha);
        }
    }

    currentImage = result;
    updateImageDisplay();
}

void MainWindow::applyColorAdjustments()
{
    if (baseImage.isNull())
        return;

    pushUndoState(currentImage);
    statusBar()->showMessage("已套用色彩調整到歷史紀錄", 3000);
}

void MainWindow::resetSliders()
{
    brightnessSlider->blockSignals(true);
    contrastSlider->blockSignals(true);
    saturationSlider->blockSignals(true);

    brightnessSlider->setValue(0);
    contrastSlider->setValue(0);
    saturationSlider->setValue(0);
    brightnessValLabel->setText("0");
    contrastValLabel->setText("0");
    saturationValLabel->setText("0");

    brightnessSlider->blockSignals(false);
    contrastSlider->blockSignals(false);
    saturationSlider->blockSignals(false);
}

void MainWindow::showOriginalPreview()
{
    if (originalImage.isNull())
        return;
    isHoldingOriginal = true;
    updateImageDisplay();
}

void MainWindow::hideOriginalPreview()
{
    if (!isHoldingOriginal)
        return;
    isHoldingOriginal = false;
    updateImageDisplay();
}

void MainWindow::updateImageDisplay()
{
    if (currentImage.isNull())
        return;

    const QImage &displayImg = isHoldingOriginal ? originalImage : currentImage;
    QPixmap pix = QPixmap::fromImage(displayImg);
    QSize targetSize = pix.size() * scaleFactor;
    imageLabel->setPixmap(pix.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->resize(targetSize);

    QString modeTag = isHoldingOriginal ? "【目前顯示：原始圖片】" : "【目前顯示：編輯結果】";
    statusBar()->showMessage(QString("%1  尺寸：%2 x %3 px  |  縮放：%4%  |  可復原：%5 步")
                                 .arg(modeTag)
                                 .arg(displayImg.width())
                                 .arg(displayImg.height())
                                 .arg(qRound(scaleFactor * 100))
                                 .arg(undoStack.size()));
}

void MainWindow::updateActionStates()
{
    bool hasImg = !baseImage.isNull();

    saveAct->setEnabled(hasImg);
    resetAct->setEnabled(hasImg);
    undoAct->setEnabled(!undoStack.isEmpty());
    redoAct->setEnabled(!redoStack.isEmpty());

    zoomInAct->setEnabled(hasImg);
    zoomOutAct->setEnabled(hasImg);
    normalSizeAct->setEnabled(hasImg);

    grayscaleAct->setEnabled(hasImg);
    invertAct->setEnabled(hasImg);
    rotateLeftAct->setEnabled(hasImg);
    rotateRightAct->setEnabled(hasImg);
    flipHAct->setEnabled(hasImg);
    flipVAct->setEnabled(hasImg);

    brightnessSlider->setEnabled(hasImg);
    contrastSlider->setEnabled(hasImg);
    saturationSlider->setEnabled(hasImg);
    compareOriginalBtn->setEnabled(hasImg);
    applyAdjustBtn->setEnabled(hasImg);
    resetSliderBtn->setEnabled(hasImg);

    quickGrayBtn->setEnabled(hasImg);
    quickInvertBtn->setEnabled(hasImg);
    quickRotateRightBtn->setEnabled(hasImg);
    quickSaveBtn->setEnabled(hasImg);
}