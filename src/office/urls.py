from django.contrib import admin
from django.urls import path
from office import views

urlpatterns = [
    path('info/', views.index, name='info'),
]

