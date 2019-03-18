### QJsonPath

`QJsonPath::getValue` gets value from QJsonDocument or QJsonValue, traversing
the given path to it first.

`QJsonPath::setValue` sets value from QJsonDocument or QJsonValue, traversing to
given path to it, and storing every tree element it finds, and then in reverse
reconstructing the json with the value changed. (It's a slow process, but if you
insist on using QJsonDocument, then fucking deal with it, because there's
basically no references or pointers)
