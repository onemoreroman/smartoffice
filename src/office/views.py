from django.shortcuts import render


# Create your views here.

def index(request):

    temp_air = [
        {'x': 1, 'y': 28.2},
        {'x': 2, 'y': 27.2},
        {'x': 3, 'y': 26.2}
    ]

    temp_heat = [
        {'x': '2018-12-01 00:00:00', 'y': 30.2},
        {'x': '2018-12-02 00:00:00', 'y': 29.2},
        {'x': '2018-12-03 00:00:00', 'y': 28.2}
    ]

    template = 'info.html'
    context = {
        'temperature_air': temp_air,
        'temperature_heater': temp_heat
    }
    return render(request, template, context)