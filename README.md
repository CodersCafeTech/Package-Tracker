# Secure Packages With AI

Safe transit of packages is a headache for all online retailers since many damages occur during transit. These damages lead to a greater number of product returns, poor customer experience, and the reputation of the retailers. Since we have no knowledge of what happens during delivery, it might be difficult to demonstrate that a package's contents were destroyed during travel as opposed to before or after transfer to a carrier. Due to the existing situation, both unfair accusation and deliberate fraud are possible. 


As a solution, we are designing a device that can monitor the safe handling of packages during transit. This device uses a Nano 33 BLE Sense, and a GSM module along with Edge Impulse to identify the locations where impetuous handling of the package occurs. Using the accelerometer data, the device can identify all possible insecure handling that occurred during the transit. Once insecure handling is detected by the device, an alert is generated in our system which logs the current time of the incident. The log can be analyzed by the retailer as part of an internal audit to analyze the performance of the courier services. The user can also track the safe handling of the package using the web interface provided. 


This device is extremely useful in transporting **handle with care** packages that should be handled very carefully.

---
# How Does It Work?

Arduino Nano 33 BLE Sense has a 9-axis IMU (3-axis accelerometer + 3-axis gyroscope + 3-axis magnetometer), which makes it ideal to recognize gestures. The movements are classified by the AI model into five classes - Hard fall, Vigorous Shaking, Throwing, Normal Carrying, and Idle - after ingesting the data from the IMU. Hard fall, Vigorous Shaking, and Throwing are categorized as insecure handling and the rest fall under secure handling. Once an insecure handling is detected by the AI model, the GSM module is activated and it logs the insecure handling event in firebase. The events can be tracked by the user and internal audit team to analyse the performance of carriers.

![Architecture.png](https://usercdn.edgeimpulse.com/project124528/fafc13e9483b6078a698c6292931a11565279e0196626834f2d3d42536c76907)

## Hardware Requirements
* Arduino Nano 33 BLE Sense
* SIM 800L GSM Module
* TP4056 Module
* Boost Converter
* Li-ion Battery


## Software Requirements
* Edge Impulse
* Arduino IDE

---
# Software Setup


## 1. Create a new project

The first and foremost step is to create a new Edge Impulse project. If you already have an account, you can create a new project by following the steps shown below. If you don't have an Edge Impulse account, sign up for a new account and follow the steps.

![Studio.png](https://usercdn.edgeimpulse.com/project129415/13a674da9ef8229b1174b12ebb223da25dd7458ca40c9b7dd802537b4e04a336)

![New Project.png](https://usercdn.edgeimpulse.com/project124528/98c6c44f79f169dc74ff6412a4b8aead2c4b0659cf45aeee2677f3cf388a8bd5)

In our case we are classifying the gestures after analysing the accelerometer data from the IMU. So make sure you choose ** Accelerometer Data **from the list

![Accelerometer.png](https://usercdn.edgeimpulse.com/project124528/b9f7b68fffb91084fbb364a6c5a8354ac2e438fd5d175da37ba42a3cf2487e4b)

## 2. Connect the Device

In order to collect acclerometer data directly from Arduino Nano 33 BLE sense, we should connect the device to the Edge Impulse studio first. Follow the steps given [here](https://docs.edgeimpulse.com/docs/development-platforms/officially-supported-mcu-targets/arduino-nano-33-ble-sense) to connect the device with the studio.

After you have completed all the above steps, make sure you see your device in the **Devices** tab.

![Devices.png](https://usercdn.edgeimpulse.com/project124528/a1a65e562c36a637ed727d91790ba4abd175e30fdb1f6fc987a639a9e0ad0de6)


## 3. Data Collection

Machine learning begins with the data you provide. It is crucial to gather clean data so that your machine learning model can identify the proper patterns. How accurate your model is will depend on the quality of the data you provide the computer. Inaccurate or out-of-date data will result in inaccurate results or predictions that are irrelevant. 

As it will directly impact the result of your model, be sure to obtain data from a reputable source. Good data is pertinent, has few duplicated and missing information, and accurately represents all of the classifications and subcategories that are there.

As we are building a motion recognition model, we are collecting the acceleromter data from IMU. 

To collect the data, navigate to **Data Acquisition** tab. 

![Data Collection.png](https://usercdn.edgeimpulse.com/project124528/af1cd149088a99bcf7d36c00af307376b6d09fd60be42b2a033fdedb6216994c)

There you can see a **Record new data** tab, where we can collect data from the supported devices connected to Edge Impulse. Setting all the data sampling parameters as shown in the figure, now it's time to collect some data. Vary the sampling parameters according to your needs.

We embedded the Arduino Nano 33 BLE Sense on a cushion to collect the required data for training.

![Data Collection Setup.jpg](https://usercdn.edgeimpulse.com/project129415/c39a9670f9d8b3ba917609331bb2d001f2498fef9ee37643ffb8fbafca5ab3d1)

## 4. Data Preperation

Finally, we have 28 minutes of accelerometer data. The data is collected under 5 different classes - Hard Fall, Vigorous Shaking, Throwing, Normal Carrying, Idle. 

After we have our data, we have to prepare it. For this we can do the following.

### 1. Visualise The Data
Visualize the data to understand how it is structured and understand the relationship between various variables that may help us in designing the impulse.

![Hard Fall.png](https://usercdn.edgeimpulse.com/project124528/f36403f8bb5f808d35517021c55502c9da7a0d3d188e864bf5fd8a500738afec)

### 2. Clean The Unwanted Data
After we visualise the data we can determine whether the data is useful for model training. Remove unwanted data to create a clean dataset.

### 3. Split The Data
Once we have a clean dataset, split it into training and testing datasets. Here we split them up into two in the ratio 80:20. If your data is not splitted, perform a test/train split either from **Data Acquisition Tab** or from **Dashboard**

![Train : Test Split.png](https://usercdn.edgeimpulse.com/project124528/ec11523a2a15c1858ba2001e2f7fb22b82927f36ddead2f37c4a4460446a06cd)

![Test : Train Split.png](https://usercdn.edgeimpulse.com/project124528/82924d3848d82309cbba7555da7eae1d1d8a058bda7ef8c12ab7deee82a10102)

## 5. Impulse Design

By creating an Impulse in Edge Impulse, you're creating your own machine learning pipeline. Navigate to **Impulse design > Create Impulse**

An impulse contain 3 blocks : *input block, processing block and a learning block*. I have chosen **Time series data** as my input block, **Spectral Analysis** as processing block and **Classification (Keras)** as learning block. You can add an anomaly block if you're interested in detecting anomalous motions, I'm just omitting it for now.

![Impulse.png](https://usercdn.edgeimpulse.com/project124528/551e3c5a6a572440b6bfeed5924eab6f17e7777e98ba51fc9b39a19255853cc3)


## 6. Feature Generation

Next, select **Save Impulse**, then navigate to the **Spectral Features** in the Impulse design panel by clicking on it. Once the parameters have been generated, wait a moment and then click **Save parameters**.

![Save Parameters.png](https://usercdn.edgeimpulse.com/project124528/7827472cda0c71d9fdd958df2b402fbae5cf4e95dabedbc2459cf2a01c75f908)

Now proceed to the **Generate features** tab and then click **Generate features**. When the process is finished, the feature explorer tab will allow you to view your dataset. This allows you to quickly verify whether your data is properly clustered or not.

![Generate Features.png](https://usercdn.edgeimpulse.com/project124528/9232649961c28786ea5e9b6fa597f88164d9b05dd13726a46e67dba368d16f64)


## 7. Model Training

Model training is the phase in which the neural network try to fit the best combination of weights and bias to a machine learning algorithm to minimize a loss function.

In the **NN Classifier** tab,under the **Impulse Design** menu, allows we can configure various parameters that influence the training process of the neural network. I have changed the default values as shown in the image to attain better accuracy.

![Model Training.png](https://usercdn.edgeimpulse.com/project124528/b74014354dc12e5f0c62a750f2a4f6b6b6961f6de7393d17aa01ee972f50ec96)

After finishing, you will see some training performance metrics, like Accuracy and Loss. Our trained model has an accuracy of 95.4%, which would suffice all our needs. 

![Accuracy.png](https://usercdn.edgeimpulse.com/project124528/e42731e07a7e9b3545bbd93f3f17ecac5677639ee64227046b8399994de5c251)

## 8. Model Testing

Once the model is trained, we can now test it to see how it performs with new data. Select **Classify All** under **Model Testing**. The model's performance on our testing data is displayed in the Model testing results tab. Our accuracy is 91.3%, which is quite good. You can also look at the Confusion matrix to determine  which labels are most susceptible to error. On the basis of this, you can expand the training dataset for these classes with additional items.

![Model Testing.png](https://usercdn.edgeimpulse.com/project124528/ed6e35956f5bc66627aa44b26ac01f4b7e1091ed53488e5595a494a1ac32839f)

In addition to classifying the the test data, we can head on to **Live Classification** to collect real-time data and classify it in real-time. This will ensure that the model is working flawlessly with real world data.

![Live Classification.png](https://usercdn.edgeimpulse.com/project124528/b6a697228f37913ab6b2d8ea3dbcc7a459a11a48077175bf56206af69548dd49)

## 9. Deploying Model To The Device

Once the model is trained and tested, it's time to deploy it back to the device. For this navigate to **Deployment > Build Firmware**. Select **Arduino Nano 33 BLE Sense** and **Build**. It will generate the model and download it as zip. Add the downloaded zip in the arduino libraries, and you are good to go.

![Deployment.png](https://usercdn.edgeimpulse.com/project124528/1d65aa0ff1436a57d5ed0d9f87e993d1e449038651d4b4fba1b1e74fea698094)

There are choices for choosing the NN classifier optimizations in Edge Impulse. We can improve device performance by choosing the best option.Edge Impulse will suggest the option that is optimal for our needs. We will achieve the same accuracy with less memory if we enable the compiler.

![Optimization.png](https://usercdn.edgeimpulse.com/project124528/e19b7b4cfa3175286ddef6e3c6378a346f6725484619b8ccd7ab04f79c8be806)

## 10. Firebase Realtime Database

Firebase is a platform for creating both mobile and online applications. Thanks to Firebase, developers may now concentrate on creating amazing user experiences.No Server management is necessary, no need to create APIs. Firebase is your server, API, and data storage, all of which are constructed in such a generic way that you can adapt them to the majority of demands. In our project, we are using Firebase real-time database to instantly post and retrieve data.

To find Firebase Config

* Go to Firebase
* Then go to Settings > Project Settings
* Under Your Apps > SDK Setup and Configuration > Config (Sample given below)
```
const firebaseConfig = {
  apiKey: "<apiKey>",
  authDomain: "<authDomain>",
  databaseURL: "<databaseURL>",
  projectId: "<projectID>",
  storageBucket: "<storageBucket>",
  messagingSenderId: "<ID>",
  appId: "<APP ID>"
};
```

## 12. Web Interface

The web interface is designed in such a way that it can reflect all the events updated in the Firebase database. The insecure handling events are updated in the firebase directly from the Nano 33 BLE Sense and other shipping updates can be updated using an API.

![Interface.png](https://usercdn.edgeimpulse.com/project124528/2e85a83062f5f1d0467e9276b514588423ccccb5b22f9a33fca285fa73b1a641)

---
# Hardware   

The heart of the **Package tracker** is an Arduino Nano 33 BLE sense. We opted for this board due to its tiny form factor and its high capability. It comes with a series of embedded sensors and here we use the LSM9DS1 sensor which is a 9-axis inertial module. It comes with one extra hardware serial port which benefits here, to connect with the GSM module.

![IMG_1737.jpg](https://usercdn.edgeimpulse.com/project109997/c4e87619c116b3b11791d52311ed1e9d286934285a316dcf7ac40952ab77f1ab)

The power supply for the device is a 3.7V 18650 Li-ion cell with a current of 2000mah. 

![IMG_1714.jpg](https://usercdn.edgeimpulse.com/project109997/a7984507e44097eb19dd8b5a5df1998e77dd844a55974259b0f58b59f30571fc)

This cell can be charged via a micro USB port. The charge controller used in this project is TP4056.  

![IMG_1762.jpg](https://usercdn.edgeimpulse.com/project109997/7e02940491a5e7d64cd77bc5396bc6617cac15dbaab6c31daee028afd65190c0)  

The power coming out of the TP4056 module is actually not enough for the **Vin** of Arduino . So we used this tiny boost converter module.

![IMG_1718.jpg](https://usercdn.edgeimpulse.com/project109997/785b1a59ff49075fdb8e6fe5db768cd492c026b4be810937b8581966f009efde)

For sending the notification to the firebase we actually used sim800l GSM module. This module is from Simcom that gives any microcontroller GSM functionality, meaning it can connect to the mobile network to receive calls and send and receive text messages, and also connect to the internet using GPRS, TCP, or IP. The communication between the Arduino and GSM module is serial.

![IMG_1713.jpg](https://usercdn.edgeimpulse.com/project109997/eead2cb1ea103622e5b67abb5253b101e556a92d233485354a2a367af5f8a4b9)

We designed and 3D printed these tiny case for securing all the hardware elements. This can be easily attached to any package using wiring ties.  

![IMG_1739.jpg](https://usercdn.edgeimpulse.com/project109997/a78c477727bc123fb50e3ed584a335073787be0ec8c46f45f49fc8d59b09d477)

Then we secured all the elements one by one in the box and tied it to the package.

![IMG_1751.jpg](https://usercdn.edgeimpulse.com/project109997/cc581397ebb547ae485b59b5d5525a30a30041593878f0ef8f6264979c65e82d)

![IMG_1756.jpg](https://usercdn.edgeimpulse.com/project109997/c7cc71100c73cd0065e79f80e4b00d72a6a503d03e0fb0a765df186c56edbbf3)

---
# Demo Video

[![Click to Play](https://usercdn.edgeimpulse.com/project129415/c7d37bc2ffa4cf0a4963801ff0c2515686c0b31eaeb4fe1ec490064e77a3fdcb)](https://vimeo.com/743090998)

---
# Code

The entire code and assets are available at : [https://github.com/CodersCafeTech/Package-Tracker](https://github.com/CodersCafeTech/Package-Tracker)
