import pickle
import random
import pandas as pd

from datetime import datetime, timedelta
from math import sqrt
from matplotlib import pyplot
from pandas import datetime
from pandas import read_csv
from sklearn.metrics import mean_squared_error


def generate_data():
    df = pd.DataFrame({'Day': [], 'Data': []})
    start_date = "01.01.2022"
    date_1 = datetime.strptime(start_date, "%d.%m.%Y")
    data = random.randint(3, 14)
    for i in range(40):
        if (i % 2 == 0):
            data = data + 8
        else:
            data = data - 4
            if (data <= 0):
                data = 1
        end_date = date_1 + timedelta(days=i)
        df.loc[i] = [end_date] + [data % 14]
    return df


def parser(x):
    return datetime.strptime(x, "%d.%m.%Y")


series = read_csv('data.csv', header=0, index_col=0, parse_dates=True, squeeze=True, date_parser=parser)
series.index = series.index.to_period('M')
# split into train and test sets
df = generate_data()
X = df.iloc[:, 1].tolist()
print(X)
size = int(len(X) * 0.66)
train, test = X[0:size], X[size:len(X)]
history = [x for x in train]
predictions = list()
# walk-forward validation

for t in range(len(test)):
    model = ARIMA(history, order=(5, 1, 0))
    output = model.fit().forecast()
    yhat = output[0]
    predictions.append(yhat)
    obs = test[t]
    history.append(obs)
    print('predicted=%f, expected=%f' % (yhat, obs))
# evaluate forecasts
print(predictions)
rmse = sqrt(mean_squared_error(test, predictions))
print('Test RMSE: %.3f' % rmse)
# plot forecasts against actual outcomes
pyplot.plot(test, color='plum', label='expected values')
pyplot.plot(predictions, color='turquoise', marker='*', linestyle='--', label='predictions')

pyplot.title('Session prediction')
pyplot.ylabel('Counter of the day since the prediction started')
pyplot.xlabel('Number of sessions')
leg = pyplot.legend()
pyplot.show()