import { NgModule } from '@angular/core';
import { RadioButtonModule } from 'primeng/radiobutton';
import { ButtonModule } from 'primeng/button';
import { ChartModule } from 'primeng/chart';
import { CheckboxModule } from 'primeng/checkbox';
import { DropdownModule } from 'primeng/dropdown';
import { FileUploadModule } from 'primeng/fileupload';
import { InputGroupModule } from 'primeng/inputgroup';
import { InputGroupAddonModule } from 'primeng/inputgroupaddon';
import { InputTextModule } from 'primeng/inputtext';
import { InputSwitchModule } from 'primeng/inputswitch';
import { SidebarModule } from 'primeng/sidebar';
import { SliderModule } from 'primeng/slider';

const primeNgModules = [
    SidebarModule,
    InputTextModule,
    InputSwitchModule,
    CheckboxModule,
    DropdownModule,
    SliderModule,
    ButtonModule,
    FileUploadModule,
    ChartModule,
    InputGroupModule,
    InputGroupAddonModule,
    RadioButtonModule
];

@NgModule({
    imports: [
        ...primeNgModules
    ],
    exports: [
        ...primeNgModules
    ],
})
export class PrimeNGModule { }
