from math import floor

import pandas as pd

from rest_framework.decorators import api_view
from rest_framework.response import Response
from django.shortcuts import render
from django.conf import settings
from rest_framework import status

from office.models import Sensors, SensorsData


@api_view(['POST'])
def upload_data(request):
    sensor_name = request.data.get('name', None)
    sensor_value = request.data.get('value', None)

    sensor = Sensors.objects.filter(name=sensor_name)
    if sensor is None:
        return bad_response('{} not in DB'.format(sensor_name))

    sensor_data = SensorsData(sensor=sensor[0])
    sensor_data.value = sensor_value
    sensor_data.save()
    return good_response(data={'value': sensor_value})


def bad_response(message, errors=None):
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
    template = 'info.html'
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
    for sensor in Sensors.objects.all().order_by('type'):
        try:
            ts = pd.read_csv('sensor'+str(sensor.id)+'_'+str(days)+'d.csv', header=None)
            ts_min = ts[1].min()
            ts_max = ts[1].max()
            ts = ts.replace({pd.np.nan: 'NaN'})
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
        'periods': [{'name': n, 'days': d} for n, d in zip(settings.PERIOD_NAMES, settings.PERIOD_NDAYS)]
    }
    return render(request, template, context)
