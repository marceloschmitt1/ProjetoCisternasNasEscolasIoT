from django.contrib import admin

from .models import Cisterna
from .models import Medicao

# Register your models here.
admin.site.register(Cisterna)
admin.site.register(Medicao)
