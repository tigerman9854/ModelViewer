#pragma once
#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QSlider>
#include <QDial>
#include <QScrollBar>
#include <QGroupBox>
#include <QStackedWidget>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>

class ViewerGraphicsWindow;
class GraphicsWindowUniform : public QWidget
{
    Q_OBJECT

public:
    GraphicsWindowUniform(ViewerGraphicsWindow* graphicsWindow, QWidget* parent = nullptr);

private:
    void createColorController32();
    void createColorController64();
    void createSettingController();
    void createEffectController();
    void createLightingController();

    //Color uniform
    QLabel* colorRLabel;
    QSlider* colorRSlider;
    QLabel* colorGLabel;
    QSlider* colorGSlider;
    QLabel* colorBLabel;
    QSlider* colorBSlider;
    QDial* colorDial;
    QGroupBox* colorGroup32;

    //Color 64 bit
    QLabel* redLabel64;
    QDoubleSpinBox* redSpinBox64;
    QLabel* greenLabel64;
    QDoubleSpinBox* greenSpinBox64;
    QLabel* blueLabel64;
    QDoubleSpinBox* blueSpinBox64;
    QDial* colorDial64;
    QGroupBox* colorGroup64;


    // Switch color format
    QStackedWidget* stackedColor;
    QComboBox* colorFormatCombo;

    //Seting uniform
    QCheckBox* lightingOn;
    QCheckBox* smoothingOn;
    QGroupBox* settingGroup;

    //effect uniform
    QComboBox* effectCombo;
    QGroupBox* effectGroup;

    //ADS Lighting features
    QLabel* ambientLabel;
    QDoubleSpinBox* ambientSpinBox;
    QLabel* diffuseLabel;
    QDoubleSpinBox* diffuseSpinBox;
    QLabel* specularLabel;
    QDoubleSpinBox* specularSpinBox;
    QGroupBox* lightGroup;

    //ModelViewer
    ViewerGraphicsWindow* m_pGraphicsWindow = nullptr;
};

//*******************
//A slider that can emit the double number as the signal
//Adapted from: https://stackoverflow.com/questions/19003369/how-to-make-a-qslider-change-with-double-values
//Author: William Spinelli
//*******************

//*******************

class DoubleSlider : public QSlider {
    Q_OBJECT

public:
    DoubleSlider(QWidget* parent = 0) : QSlider(parent) {
        connect(this, SIGNAL(valueChanged(int)),
            this, SLOT(notifyValueChanged(int)));
    }

signals:
    void doubleValueChanged(double value);

public slots:
    void notifyValueChanged(int value) {
        double doubleValue = value / 10.0;
        emit doubleValueChanged(doubleValue);
    }
};