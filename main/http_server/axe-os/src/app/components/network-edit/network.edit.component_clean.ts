import { HttpClient, HttpErrorResponse } from '@angular/common/http';
import { Component, Input, OnInit, OnDestroy } from '@angular/core';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { ToastrService } from 'ngx-toastr';
import { finalize, startWith, switchMap, takeUntil } from 'rxjs/operators';
import { BehaviorSubject, Observable, Subject, interval } from 'rxjs';
import { DialogService } from 'src/app/services/dialog.service';
import { LoadingService } from 'src/app/services/loading.service';
import { SystemService } from 'src/app/services/system.service';
