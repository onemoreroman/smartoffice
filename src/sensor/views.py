from math import floor

import pandas as pd
import numpy as np

from rest_framework.decorators import api_view
from rest_framework.response import Response
from django.shortcuts import render
from django.conf import settings
from rest_framework import status

from sensor.models import Sensor, SensorData


@api_view(['POST'])
def upload_data(request):
    try:
        sensor_name = request.data.get('sensor_name', None)
        sensor_value = request.data.get('sensor_value', None)
        assert(len(sensor_name) > 0)
        assert(sensor_value > -99999999)
    except:
        return bad_response('Bad json {}'.format(str(request.data)))

    sensor = Sensor.objects.filter(name=sensor_name)
    if not sensor:
        return bad_response('Sensor {} not in DB, {}'.format(sensor_name, sensor))

    sensor_data = SensorData(sensor=sensor[0])
    sensor_data.value = sensor_value
    sensor_data.save()
    return good_response(data={'value': sensor_value})


def bad_response(message, errors=None):
    print(message)
    response = Response({
        'data': {
            'success': False,
            'message': message,
            'errors': {}
        }
    }, status=status.HTTP_400_BAD_REQUEST)
    if errors:
        response.data['data']['errors'] = errors
    return response


def good_response(data=None):
    response = Response({
        'data': {
            'success': True,
        }
    })
    if data:
        for item, value in data.items():
            response.data['data'][item] = value
    return response


def days(request, days=0):
    template = 'display.html'
    charts = []
    if days == 0:
        scale_units = 'minute'
    elif days == 1:
        scale_units = 'hour'
    elif 1 < days < 120:
        scale_units = 'day'
    else:
        scale_units = 'month'
    i = 0
    for sensor in Sensor.objects.filter(active=True).order_by('type'):
        try:
            ts = pd.read_csv('sensor/csv/sensor'+str(sensor.id)+'_'+str(days)+'d.csv', header=None)
            ts_min = ts[1].min()
            ts_max = ts[1].max()
            ts = ts.replace({np.nan: 'NaN'})
        except:
            ts = pd.DataFrame([], columns=[0, 1])
            ts_min = sensor.display_min
            ts_max = sensor.display_max

        charts.append({
            'name': sensor.display_name,
            'units': sensor.units,
            'data': ts.rename(columns={0: 'x', 1: 'y'}).to_dict('records'),
            'x_units': scale_units,
            'y_max': floor(max(sensor.display_max, ts_max)),
            'y_min': floor(min(sensor.display_min, ts_min)),
            'id': 'chart' + str(i)
        })
        i += 1
    context = {
        'charts': charts,
        'periods': [{'name': n, 'days': int(d)} for n, d in zip(settings.PERIOD_NAMES, settings.PERIOD_NDAYS)]
    }
    return render(request, template, context)
