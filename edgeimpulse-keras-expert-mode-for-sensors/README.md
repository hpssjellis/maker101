# Arduino Portenta Maker101 Course

By Jeremy Ellis  
Started July 6th, 2022  
Use at your own Risk!  

Basic Edge Impulse Forum question [here]https://forum.edgeimpulse.com/t/generic-sensor-raw-data-to-keras-classification/4902/3)







.








.




Random info that I may be working with


```
def make_model(input_shape):
    input_layer = keras.layers.Input(input_shape)

    conv1 = keras.layers.Conv1D(filters=64, kernel_size=3, padding="same")(input_layer)
    conv1 = keras.layers.BatchNormalization()(conv1)
    conv1 = keras.layers.ReLU()(conv1)

    conv2 = keras.layers.Conv1D(filters=64, kernel_size=3, padding="same")(conv1)
    conv2 = keras.layers.BatchNormalization()(conv2)
    conv2 = keras.layers.ReLU()(conv2)

    conv3 = keras.layers.Conv1D(filters=64, kernel_size=3, padding="same")(conv2)
    conv3 = keras.layers.BatchNormalization()(conv3)
    conv3 = keras.layers.ReLU()(conv3)

    gap = keras.layers.GlobalAveragePooling1D()(conv3)

    output_layer = keras.layers.Dense(num_classes, activation="softmax")(gap)

    return keras.models.Model(inputs=input_layer, outputs=output_layer)


model = make_model(input_shape=x_train.shape[1:])
keras.utils.plot_model(model, show_shapes=True)
```




Switch of plans, just using acceleration or any raw keras model load these layers see if it compiles and then try to prune the netwro down to fit better on your device

![image](https://user-images.githubusercontent.com/5605614/178207452-e64fb3c6-30ba-4957-9c84-c27b011c55cc.png)






