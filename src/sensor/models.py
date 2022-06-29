from django.db import models
from django.contrib import admin


class Sensor(models.Model):
    name = models.CharField(max_length=128)
    location = models.CharField(max_length=128, default='')
    type = models.IntegerField(default=0)
    units = models.CharField(default='', max_length=16, null=True, blank=True)
    display_name = models.CharField(max_length=128, default='')
    display_min = models.FloatField(default=0.0)
    display_max = models.FloatField(default=100.0)
    active = models.BooleanField(default=True)


class SensorData(models.Model):
    sensor = models.ForeignKey('sensor.Sensor', on_delete=models.CASCADE)
    value = models.CharField(default='', max_length=16, null=True, blank=True)
    time = models.DateTimeField(blank=False)


class SensorAdmin(admin.ModelAdmin):
    list_display = ('name', 'location', 'units', 'active')

