from django.test import TestCase

from sensor.models import Sensor, SensorData


class MyTests(TestCase):
    @classmethod
    def setUpTestData(cls):
        test_sensor = Sensor.objects.create(name='test_sensor')

    def test_upload_data(self):
        test_data = {'sensor_name': 'test_sensor', 'sensor_value': 2342.234}
        response = self.client.post('/sensor/upload_data', test_data, content_type='application/json')
        self.assertEqual(response.status_code, 200)
