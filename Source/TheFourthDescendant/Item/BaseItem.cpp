#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;
    
	// 루트 컴포넌트 생성 및 설정
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	// 충돌 컴포넌트 생성 및 설정
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);
    
	// 스태틱 메시 컴포넌트 생성 및 설정
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Collision);

	// 물리 시뮬레이션과 중력 활성화
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetEnableGravity(true);
    
	// StaticMesh가 실제로 충돌할 수 있도록 충돌 활성화 변경
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// 필요에 따라 Collision Response를 설정할 수도 있음.
	// 예: StaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	// Overlap 이벤트 바인딩
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnItemEndOverlap);
}


void ABaseItem::OnItemOverlap(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, 
			bool bFromSweep, 
			const FHitResult& SweepResult)
{
	  // OtherActor가 플레이어인지 확인 ("Player" 태그 활용)
    if (OtherActor && OtherActor->ActorHasTag("Player"))
    {
        ActivateItem(OtherActor);
    }
}

void ABaseItem::OnItemEndOverlap(
			UPrimitiveComponent* OverlappedComp, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex)
{
}

void ABaseItem::ActivateItem(AActor* Activator)
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation());
	}

	if(NiagaraActor)
	{
		FActorSpawnParameters SpawnParams;
		GetWorld()->SpawnActor<AActor>(NiagaraActor, 
				GetActorLocation(), 
				FRotator::ZeroRotator, 
				SpawnParams);
	}
}

FName ABaseItem::GetItemType() const
{
    return ItemType;
}

void ABaseItem::DestroyItem()
{
    Destroy();
}
