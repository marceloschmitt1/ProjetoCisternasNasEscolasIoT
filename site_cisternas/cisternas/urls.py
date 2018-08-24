from django.contrib import admin
from django.urls import path
from rest_framework.urlpatterns import format_suffix_patterns
# from django.conf.url import url
from cisternas import views

# from . import views

app_name = 'cisternas'
urlpatterns = [
    path('', views.cisternas_list, name='list'),
    path('<slug:cisterna_slug>', views.cisternas_detail, name='detail'),
    path('teste_api/', views.CisternaList.as_view()),
    path('teste_api/<int:pk>/', views.CisternaDetail.as_view()),
    path('user/', views.UserList.as_view()),
    path('user/<int:pk>', views.UserDetail.as_view())
    # path('teste_api/(?P<pk>[0-9]+)/$', views.cis_detail)
    # path('api-auth/', include('rest_framework.urls')),
]

urlpatterns = format_suffix_patterns(urlpatterns)