#include "GraphicsWindowDelegate.h"
#include "ViewerGraphicsWindow.h"
#include "UniformController.h"

#include <QGridLayout>
#include <QLabel>

GraphicsWindowUniform::GraphicsWindowUniform(ViewerGraphicsWindow* graphicsWindow, QWidget* parent)
    : m_pGraphicsWindow(graphicsWindow), QWidget(parent)
{
    //Not all types need to be used when implement the unifrom
    //They are just 4common UIs
    //template of slider and dial, example: uniform varaiable color RGB
    createColorController32();
    createColorController64();
    ///Switch 2 formats color
    colorFormatCombo = new QComboBox;
    colorFormatCombo->addItem(tr("Using 32-bits Color"));
    colorFormatCombo->addItem(tr("Using 64-bits Color"));
    stackedColor = new QStackedWidget;
    stackedColor->addWidget(colorGroup32);
    stackedColor->addWidget(colorGroup64);
    connect(colorFormatCombo, QOverload<int>::of(&QComboBox::activated),
       stackedColor, &QStackedWidget::setCurrentIndex);

    //template of checkBox, example: lighting(On/Off), smoothing...
    createSettingController();
    //template of choice box exmaple: image effect
    createEffectController();
    //template of Spin box, example: ADS lighting features
    createLightingController();

    //Windows layout
    QVBoxLayout* layout = new QVBoxLayout;
    //layout->addWidget(colorGroup32);
    //layout->addWidget(colorGroup64);
    layout->addWidget(stackedColor);
    layout->addWidget(colorFormatCombo);
    layout->addWidget(settingGroup);
    layout->addWidget(effectGroup);
    layout->addWidget(lightGroup);
    setLayout(layout);

    setWindowTitle(tr("Uniform"));
}

void GraphicsWindowUniform::createColorController32() {
    colorGroup32 = new QGroupBox("Color");

    //RGB color
    colorRLabel = new QLabel(tr("Red"));
    colorRSlider = new QSlider(Qt::Horizontal);
    colorRSlider->setFocusPolicy(Qt::StrongFocus);
    colorRSlider->setTickPosition(QSlider::TicksBothSides);
    colorRSlider->setTickInterval(10);
    colorRSlider->setSingleStep(0.01);
    colorRSlider->setMaximum(255);
    colorRSlider->setMinimum(0);

    colorGLabel = new QLabel(tr("Green"));
    colorGSlider = new QSlider(Qt::Horizontal);
    colorGSlider->setFocusPolicy(Qt::StrongFocus);
    colorGSlider->setTickPosition(QSlider::TicksBothSides);
    colorGSlider->setTickInterval(10);
    colorGSlider->setSingleStep(0.01);
    colorGSlider->setMaximum(255);
    colorGSlider->setMinimum(0);

    colorBLabel = new QLabel(tr("Blue"));
    colorBSlider = new QSlider(Qt::Horizontal);
    colorBSlider->setFocusPolicy(Qt::StrongFocus);
    colorBSlider->setTickPosition(QSlider::TicksBothSides);
    colorBSlider->setTickInterval(10);
    colorBSlider->setSingleStep(0.01);
    colorBSlider->setMaximum(255);
    colorBSlider->setMinimum(0);

    // not finish
    colorDial = new QDial;
    colorDial->setFocusPolicy(Qt::StrongFocus);

    //Singal
    //Need to redo, The Qslider only emits int signal
    connect(colorRSlider, &QSlider::valueChanged, 
        m_pGraphicsWindow, &ViewerGraphicsWindow::colorRChanged);
    connect(colorGSlider, &QSlider::valueChanged, 
        m_pGraphicsWindow, &ViewerGraphicsWindow::colorGChanged);
    connect(colorBSlider, &QSlider::valueChanged, 
        m_pGraphicsWindow, &ViewerGraphicsWindow::colorBChanged);

    //set the layout
    QGridLayout* colorLayout = new QGridLayout;
    colorLayout->addWidget(colorRLabel, 0, 0, 1, 1);
    colorLayout->addWidget(colorRSlider, 0, 1, 1, 4);
    colorLayout->addWidget(colorGLabel, 1, 0, 1, 1);
    colorLayout->addWidget(colorGSlider, 1, 1, 1, 4);
    colorLayout->addWidget(colorBLabel, 2, 0, 1, 1);
    colorLayout->addWidget(colorBSlider, 2, 1, 1, 4);
    colorLayout->addWidget(colorDial, 3, 0, 2, 5);
    colorGroup32->setLayout(colorLayout);
}

void GraphicsWindowUniform::createColorController64()
{
    colorGroup64 = new QGroupBox("64-bit Color");

    redLabel64 = new QLabel(tr("Red:"));
    redSpinBox64 = new QDoubleSpinBox;
    redSpinBox64->setRange(0, 255);
    redSpinBox64->setValue(0);
    redSpinBox64->setSingleStep(0.01);

    greenLabel64 = new QLabel(tr("Green:"));
    greenSpinBox64 = new QDoubleSpinBox;
    greenSpinBox64->setRange(0, 255);
    greenSpinBox64->setValue(0);
    greenSpinBox64->setSingleStep(0.01);

    blueLabel64 = new QLabel(tr("Blue"));
    blueSpinBox64 = new QDoubleSpinBox;
    blueSpinBox64->setRange(0, 255);
    blueSpinBox64->setValue(0);
    blueSpinBox64->setSingleStep(0.01);

    //Signal 
    connect(redSpinBox64, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        m_pGraphicsWindow, &ViewerGraphicsWindow::colorRChanged64);
    connect(greenSpinBox64, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        m_pGraphicsWindow, &ViewerGraphicsWindow::colorGChanged64);
    connect(blueSpinBox64, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        m_pGraphicsWindow, &ViewerGraphicsWindow::colorBChanged64);

    // not finish
    colorDial64 = new QDial;
    colorDial64->setFocusPolicy(Qt::StrongFocus);

    //set the layout
    QGridLayout* colorLayout = new QGridLayout;
    colorLayout->addWidget(redLabel64, 0, 0, 1, 1);
    colorLayout->addWidget(redSpinBox64, 0, 1, 1, 4);
    colorLayout->addWidget(greenLabel64, 1, 0, 1, 1);
    colorLayout->addWidget(greenSpinBox64, 1, 1, 1, 4);
    colorLayout->addWidget(blueLabel64, 2, 0, 1, 1);
    colorLayout->addWidget(blueSpinBox64, 2, 1, 1, 4);
    colorLayout->addWidget(colorDial64, 3, 0, 2, 5);
    colorGroup64->setLayout(colorLayout);
}

void GraphicsWindowUniform:: createSettingController() {
    settingGroup = new QGroupBox("Setting");

    //CheckBoxes for confirming whether to turn on smoothing and lighting
    smoothingOn = new QCheckBox(tr("Smoothing"));
    lightingOn = new QCheckBox(tr("Lighting"));

    //signal
    connect(smoothingOn, &QCheckBox::toggled, 
        m_pGraphicsWindow, &ViewerGraphicsWindow::smoothingSwitch);
    connect(lightingOn, &QCheckBox::toggled,
        m_pGraphicsWindow, &ViewerGraphicsWindow::lightingSwitch);

    //layout
    QGridLayout* settingLayout = new QGridLayout;
    settingLayout->addWidget(smoothingOn);
    settingLayout->addWidget(lightingOn);
    settingGroup->setLayout(settingLayout);
}

void GraphicsWindowUniform::createEffectController() 
{
    effectGroup = new QGroupBox("Image effect");

    //A drop-down menu for users to determine the effect that will be used during the rendering
    effectCombo = new QComboBox;
    effectCombo->addItem(tr("Negative"));
    effectCombo->addItem(tr("Saturation"));
    effectCombo->addItem(tr("Sharpening"));

    //singal
    connect(effectCombo, QOverload<int>::of(&QComboBox::activated), 
        m_pGraphicsWindow, &ViewerGraphicsWindow::effectType);

    //layout
    QGridLayout* effectLayout = new QGridLayout;
    effectLayout->addWidget(effectCombo);
    effectGroup->setLayout(effectLayout);
}

void GraphicsWindowUniform::createLightingController() 
{
    lightGroup = new QGroupBox("ADS Lighting");

    //Boxes that can be filled with numbers, used for lighting uniform
    ambientLabel = new QLabel(tr("Ambient value:"));
    ambientSpinBox = new QDoubleSpinBox;
    ambientSpinBox->setRange(0, 5);
    ambientSpinBox->setValue(0);
    ambientSpinBox->setSingleStep(0.01);

    diffuseLabel = new QLabel(tr("Difusse value:"));
    diffuseSpinBox = new QDoubleSpinBox;
    diffuseSpinBox->setRange(0, 5);
    diffuseSpinBox->setValue(0);
    diffuseSpinBox->setSingleStep(0.01);

    specularLabel = new QLabel(tr("Specular value:"));
    specularSpinBox = new QDoubleSpinBox;
    specularSpinBox->setRange(0, 5);
    specularSpinBox->setValue(0);
    specularSpinBox->setSingleStep(0.01);

    //Signal 
    connect(ambientSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
        m_pGraphicsWindow, &ViewerGraphicsWindow::lightAmbient);
    connect(diffuseSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
        m_pGraphicsWindow, &ViewerGraphicsWindow::lightDiffuse);
    connect(specularSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
        m_pGraphicsWindow, &ViewerGraphicsWindow::lightSpecular);


    // Layout
    QGridLayout* ADSLightingLayout = new QGridLayout;
    ADSLightingLayout->addWidget(ambientLabel, 0, 0);
    ADSLightingLayout->addWidget(ambientSpinBox, 0, 1);
    ADSLightingLayout->addWidget(diffuseLabel, 1, 0);
    ADSLightingLayout->addWidget(diffuseSpinBox, 1, 1);
    ADSLightingLayout->addWidget(specularLabel, 2, 0);
    ADSLightingLayout->addWidget(specularSpinBox, 2, 1);
    lightGroup->setLayout(ADSLightingLayout);
}
