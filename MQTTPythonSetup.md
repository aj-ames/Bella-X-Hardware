# Setting up MQTT support for Python using PahoMQTT

### 1. Setting up a virtual environment

Yes, using a virtual environment for python is better cause you wouldn't want your system python dependencies messed up.

```sh
# To install pip
sudo apt-get install python3-pip

# To install virtualenv and its wrapper
sudo pip3 install virtualenv virtualenvwrapper
```

Add the following lines to .bashrc (.zshrc if you use zsh) by running the following commands:

```sh
echo '# virtualenv and virtualenvwrapper' >> .bashrc
echo 'export VIRTUALENVWRAPPER_PYTHON=/usr/bin/python3.6' >> .bashrc
echo 'export WORKON_HOME=$HOME/.virtualenvs' >> .bashrc
echo 'source /usr/local/bin/virtualenvwrapper.sh' >> .bashrc
```

Creating a virtual environment:
```sh
mkvirtualenv nameofenv -p python3.6
```
Using a virtual environment:
```sh
workon nameofenv
```
Deactivating a virtual environment:
```sh
deactivate
```

## 2. Installing PahoMQTT

PahoMQTT is the library used for all MQTT communication

Run your virtul environment and enter the following command:
```sh
pip3 install paho-mqtt
```
