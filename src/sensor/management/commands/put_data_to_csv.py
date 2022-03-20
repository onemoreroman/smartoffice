import os
from datetime import datetime, timedelta, timezone

from django.core.management.base import BaseCommand

import pandas as pd

from sensor.models import SensorData, Sensor


class Command(BaseCommand):
    help = 'Put sensors data to CSV'

    def add_arguments(self, parser):
        parser.add_argument('delta_days', type=int)
        parser.add_argument('--sample_mins', default=0, type=int)

    def handle(self, *args, **options):
        sensors = Sensor.objects.filter(active=True)
        if options['delta_days'] == 0:
            dt0 = datetime.now(tz=timezone.utc) - timedelta(hours=1)
        else:
            dt0 = datetime.now(tz=timezone.utc) - timedelta(days=options['delta_days'])
        for i, sensor in enumerate(sensors):
            os.makedirs('sensor/csv', exist_ok=True)
            csv_file = 'sensor/csv/sensor' + str(sensor.id) + '_' + str(options['delta_days']) + 'd.csv'
            q = SensorData.objects.filter(sensor=sensor, time__gte=dt0)
            if len(q) < 1:
                open(csv_file, 'w').close()
                continue
            time = [_.time for _ in q]
            vals = [float(_.value) for _ in q]
            ts = pd.Series(vals, index=pd.to_datetime(time))
            if options['sample_mins']:
                ts = ts.resample(str(options['sample_mins'])+'T').mean()
            ts.index = ts.index.tz_convert('Europe/Moscow').strftime('%y-%m-%d %H:%M:%S')
            ts.to_csv(csv_file, header=False)
