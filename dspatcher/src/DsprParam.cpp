#include <DsprParam.h>

DsprParam::DsprParam(int compId, int paramId, std::string const& name, DspParameter const& param, QMenu* contextMenu)
    : _compId(compId)
    , _paramId(paramId)
    , _param(param)
    , _contextMenu(contextMenu)
{
    if (_param.Type() == DspParameter::Bool)
    {
        _checkbox = new QCheckBox(_contextMenu);
        _checkbox->setText(name.c_str());
        _checkbox->setEnabled(false);
        QWidgetAction* customAction = new QWidgetAction(_contextMenu);
        customAction->setDefaultWidget(_checkbox);
        _action = customAction;

        connect(_checkbox, SIGNAL(stateChanged(int)), this, SLOT(paramChanged(int)));
    }
    else if (_param.Type() == DspParameter::Int)
    {
        QWidget* intSlider = new QWidget(_contextMenu);

        _slider = new QSlider(Qt::Horizontal, intSlider);
        if (_param.GetIntRange())
        {
            _slider->setRange(_param.GetIntRange()->first, _param.GetIntRange()->second);
        }
        else
        {
            _slider->setRange(0, *_param.GetInt() * 2);
        }
        _slider->setValue(*_param.GetInt());

        QLabel* label = new QLabel(intSlider);
        label->setText(name.c_str());

        QLabel* vlabel = new QLabel(intSlider);
        vlabel->setNum(_slider->sliderPosition());
        vlabel->setFixedWidth(45);

        connect(_slider, SIGNAL(valueChanged(int)), vlabel, SLOT(setNum(int)));

        QHBoxLayout* layout = new QHBoxLayout(intSlider);
        layout->addWidget(label);
        layout->addWidget(_slider);
        layout->addWidget(vlabel);

        QWidgetAction* intSliderAction = new QWidgetAction(_contextMenu);
        intSliderAction->setDefaultWidget(intSlider);
        _action = intSliderAction;

        connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(paramChanged(int)));
    }
    else if (_param.Type() == DspParameter::Float)
    {
        QWidget* floatSlider = new QWidget(_contextMenu);

        _slider = new QSlider(Qt::Horizontal, floatSlider);
        if (_param.GetFloatRange())
        {
            _slider->setRange(_param.GetFloatRange()->first * 100, _param.GetFloatRange()->second * 100);
        }
        else
        {
            _slider->setRange(0, *_param.GetFloat() * 200);
        }
        _slider->setValue(*_param.GetFloat() * 100);

        QLabel* label = new QLabel(floatSlider);
        label->setText(name.c_str());

        _vlabel = new QLabel(floatSlider);
        _vlabel->setNum(_slider->sliderPosition() / 100);
        _vlabel->setFixedWidth(45);

        connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(updateFloatSlider(int)));

        QHBoxLayout* layout = new QHBoxLayout(floatSlider);
        layout->addWidget(label);
        layout->addWidget(_slider);
        layout->addWidget(_vlabel);

        QWidgetAction* intSliderAction = new QWidgetAction(_contextMenu);
        intSliderAction->setDefaultWidget(floatSlider);
        _action = intSliderAction;

        connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(paramChanged(int)));
    }
    else if (_param.Type() == DspParameter::String)
    {
        QWidget* textBox = new QWidget(_contextMenu);

        QLabel* label = new QLabel(textBox);
        label->setText(name.c_str());

        _textBox = new QLineEdit(textBox);
        _textBox->setText(_param.GetString()->c_str());

        QHBoxLayout* layout = new QHBoxLayout(textBox);
        layout->addWidget(label);
        layout->addWidget(_textBox);

        QWidgetAction* customAction = new QWidgetAction(_contextMenu);
        customAction->setDefaultWidget(textBox);
        _action = customAction;

        connect(_textBox, SIGNAL(textChanged(QString const&)), this, SLOT(paramChanged(QString const&)));
    }
    else if (_param.Type() == DspParameter::FilePath)
    {
        QWidget* fileBrowser = new QWidget(_contextMenu);

        QLabel* label = new QLabel(fileBrowser);
        label->setText(name.c_str());

        _textBox = new QLineEdit(fileBrowser);
        _textBox->setEnabled(false);
        _textBox->setText(_param.GetString()->c_str());

        QPushButton* btnBrowse = new QPushButton(fileBrowser);
        btnBrowse->setText("Browse");

        QHBoxLayout* layout = new QHBoxLayout(fileBrowser);
        layout->addWidget(label);
        layout->addWidget(_textBox);
        layout->addWidget(btnBrowse);

        QWidgetAction* fileBrowserAction = new QWidgetAction(_contextMenu);
        fileBrowserAction->setDefaultWidget(fileBrowser);
        _action = fileBrowserAction;

        connect(btnBrowse, SIGNAL(released()), this, SLOT(browseFiles()));
        connect(_textBox, SIGNAL(textChanged(QString const&)), this, SLOT(paramChanged(QString const&)));
    }
    else if (_param.Type() == DspParameter::List)
    {
        QWidget* listBox = new QWidget(_contextMenu);

        QLabel* label = new QLabel(listBox);
        label->setText(name.c_str());

        _listBox = new QComboBox(listBox);
        for (size_t i = 0; i < _param.GetList()->size(); ++i)
        {
            _listBox->addItem((*_param.GetList())[i].c_str());
        }

        QHBoxLayout* layout = new QHBoxLayout(listBox);
        layout->addWidget(label);
        layout->addWidget(_listBox);

        QWidgetAction* customAction = new QWidgetAction(_contextMenu);
        customAction->setDefaultWidget(listBox);
        _action = customAction;

        connect(_listBox, SIGNAL(currentIndexChanged(int)), this, SLOT(paramChanged(int)));
    }
    else if (_param.Type() == DspParameter::Trigger)
    {
        _button = new QPushButton(_contextMenu);
        _button->setText(name.c_str());

        QWidgetAction* customAction = new QWidgetAction(_contextMenu);
        customAction->setDefaultWidget(_button);
        _action = customAction;

        connect(_button, SIGNAL(released()), this, SLOT(paramChanged()));
    }
}

QWidgetAction* DsprParam::action()
{
    return _action;
}

DspParameter& DsprParam::param()
{
    return _param;
}

int DsprParam::compId()
{
    return _compId;
}

int DsprParam::paramId()
{
    return _paramId;
}

bool DsprParam::SetBool(bool const& value)
{
    bool result = _param.SetBool(value);
    if (result && _param.Type() == DspParameter::Bool)
    {
        _checkbox->setEnabled(value);
    }
    return result;
}

bool DsprParam::SetInt(int const& value)
{
    bool result = _param.SetInt(value);
    if (result && _param.Type() == DspParameter::Int)
    {
        _slider->setValue(value);
    }
    else if (result && _param.Type() == DspParameter::List)
    {
        _listBox->setCurrentIndex(value);
    }
    return result;
}

bool DsprParam::SetIntRange(std::pair<int, int> const& intRange)
{
    bool result = _param.SetIntRange(intRange);
    if (result && _param.Type() == DspParameter::Int)
    {
        _slider->setRange(intRange.first, intRange.second);
    }
    return result;
}

bool DsprParam::SetFloat(float const& value)
{
    bool result = _param.SetFloat(value);
    if (result && _param.Type() == DspParameter::Float)
    {
        _slider->setValue(value * 100);
    }
    return result;
}

bool DsprParam::SetFloatRange(std::pair<float, float> const& floatRange)
{
    bool result = _param.SetFloatRange(floatRange);
    if (result && _param.Type() == DspParameter::Float)
    {
        _slider->setRange(floatRange.first * 100, floatRange.second * 100);
    }
    return result;
}

bool DsprParam::SetString(std::string const& value)
{
    bool result = _param.SetString(value);
    if (result && _param.Type() == DspParameter::String)
    {
        _textBox->setText(value.c_str());
    }
    else if (result && _param.Type() == DspParameter::FilePath)
    {
        _textBox->setText(value.c_str());
    }
    else if (result && _param.Type() == DspParameter::List)
    {
        for (int i = 0; i < _listBox->children().size(); ++i)
        {
            if (_listBox->itemText(i) == value.c_str())
            {
                _listBox->setCurrentIndex(i);
                break;
            }
        }
    }
    return result;
}

bool DsprParam::SetList(std::vector<std::string> const& value)
{
    bool result = _param.SetList(value);
    if (result && _param.Type() == DspParameter::List)
    {
        _listBox->clear();
        for (size_t i = 0; i < value.size(); ++i)
        {
            _listBox->addItem(value[i].c_str());
        }
    }
    return result;
}

void DsprParam::paramChanged(int value)
{
    if (_param.Type() == DspParameter::Bool)
    {
        emit boolUpdated(value != 0);
    }
    else if (_param.Type() == DspParameter::Int)
    {
        emit intUpdated(value);
    }
    else if (_param.Type() == DspParameter::Float)
    {
        emit floatUpdated((float)value / 100.f);
    }
    else if (_param.Type() == DspParameter::List)
    {
        emit intUpdated(value);
    }
}

void DsprParam::paramChanged(QString const& newString)
{
    if (_param.Type() == DspParameter::String)
    {
        emit stringUpdated(newString.toStdString());
    }
    else if (_param.Type() == DspParameter::FilePath)
    {
        emit stringUpdated(newString.toStdString());
    }
}

void DsprParam::paramChanged()
{
    if (_param.Type() == DspParameter::Trigger)
    {
        emit triggerUpdated();
    }
}

void DsprParam::updateFloatSlider(int value)
{
    _vlabel->setNum((float)value / 100);
}

void DsprParam::browseFiles()
{
    QFileDialog* custom = new QFileDialog();
    _textBox->setText(custom->getOpenFileName());
    delete custom;
}
