from datetime import datetime, timedelta, timezone

from django.core.management.base import BaseCommand, CommandError

import pandas as pd

from office.models import SensorsData, Sensors


class Command(BaseCommand):
    help = 'Put sensors data to CSV'

    def add_arguments(self, parser):
        parser.add_argument('delta_days', type=int)
        parser.add_argument('--sample_mins', default=0, type=int)

    def handle(self, *args, **options):
        sensors = Sensors.objects.all()
        dt0 = datetime.now(tz=timezone.utc) - timedelta(days=options['delta_days'])
        for i, sensor in enumerate(sensors):
            q = SensorsData.objects.filter(sensor=sensor, time__gte=dt0)
            time = [_.time for _ in q]
            vals = [float(_.value) for _ in q]
            ts = pd.Series(vals, index=pd.to_datetime(time))
            if options['sample_mins']:
                ts = ts.resample(str(options['sample_mins'])+'T').mean()
            ts.index = ts.index.tz_convert('Asia/Krasnoyarsk').strftime('%y-%m-%d %H:%M:%S')
            ts.to_csv('sensor'+str(sensor.id)+'_'+str(options['delta_days'])+'d.csv', header=False)
