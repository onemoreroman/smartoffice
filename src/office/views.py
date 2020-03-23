import pandas as pd

from rest_framework.decorators import api_view
from rest_framework.response import Response
from django.shortcuts import render
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


def index(request):
    template = 'info.html'
    charts = []
    periods = [
        ('day', 1, 'hour'),
        ('week', 7, 'day'),
        ('month', 30, 'day'),
    ]
    i = 0
    for sensor in Sensors.objects.all():
        for (period, days, scale_unit) in periods:
            try:
                ts = pd.read_csv('sensor'+str(sensor.id)+'_'+str(days)+'d.csv', header=None)
                ts = ts.replace({pd.np.nan: 'NaN'})
            except:
                ts = pd.DataFrame([], columns=[0, 1])

            charts.append({
                'name': sensor.name,
                'units': sensor.units,
                'location': sensor.location,
                'data': ts.rename(columns={0: 'x', 1: 'y'}).to_dict('records'),
                'x_units': scale_unit,
                'period': period,
                'id': 'chart' + str(i)
            })
            i += 1
    context = {'charts': charts}
    return render(request, template, context)
