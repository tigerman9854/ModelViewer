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
    void createColorController();
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
    QGroupBox* colorGroup;

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