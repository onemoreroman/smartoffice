from rest_framework.decorators import api_view
from rest_framework.response import Response
from rest_framework import status

from office.models import Sensors, SensorsData


@api_view(['POST'])
def upload_data(request):
    sensor_name = request.data.get('sensor_name', None)
    sensor_value = request.data.get('sensor_value', None)

    sensor = Sensors.objects.filter(name=sensor_name)
    if sensor is None:
        return bad_response('{} not in DB'.format(sensor_name))

    sensor_data = SensorsData(sensor=sensor[0])
    sensor_data.value = sensor_value
    sensor_data.save()
    return good_response('Data uploaded', {'sensor': sensor_name})


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


def good_response(message, data=None):
    response = Response({
        'data': {
            'success': True,
            'message': message,
            'data': {}
        }
    })
    if data:
        for item, value in data.items():
            response.data['data'][item] = value
    return response
